#include "SDFMix.h"
SDFMix::SDFMix(std::string name) :name(name)
{
}

SDFMix::~SDFMix()
{
	Destroy();
}

void SDFMix::Init()
{

	mainCamera = std::make_unique<KEngine::Camera>();
	{
		char* vertexSrc = R"(
					#version 420 core
					layout(location=0) in vec3 v_Position;
				
					uniform mat4 model;
					layout(std140,binding = 0) uniform VPMatrix
					{
						mat4 view;
						mat4 proj;
					};

					void main()
					{
						gl_Position = proj * view * model * vec4(v_Position,1.0);
					}
				)";
		char* fragmentSrc = R"(
					#version 420 core
					out vec4 color;
				
					void main()
					{
						color = vec4(1.0f);
					}

				)";
		l_Shader.reset(new KEngine::Shader(vertexSrc, fragmentSrc));
	}

	// create a fullscreen quad and SDF shader (raymarch two spheres blending)
	float quad_Vertices[] = {
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };
    unsigned int quadIndexes[] = { 0,1,2, 3,4,5 };
    KEngine::BufferLayout quad_Layout = { {KEngine::ShaderDataType::Float2, "position"}, {KEngine::ShaderDataType::Float2, "texCoords"} };

    quadMesh.reset(new KEngine::Mesh(quad_Vertices, sizeof(quad_Vertices) / sizeof(float), quad_Layout, quadIndexes, sizeof(quadIndexes) / sizeof(unsigned int)));

    const char* vs = R"(#version 420 core
    layout(location = 0) in vec2 position;
    layout(location = 1) in vec2 texCoords;
    out vec2 TexCoords;
    void main(){ gl_Position = vec4(position,0.0,1.0); TexCoords = texCoords; }
    )";
    const char* fs = R"(#version 420 core
    out vec4 FragColor;
    in vec2 TexCoords;

    uniform mat4 invProj;
    uniform mat4 invView;
    uniform vec3 camPos;
    uniform float iTime;
    uniform int uSteps;
    uniform float uMaxDist;
    uniform float uEpsilon;
    uniform float uK;

    float sdSphere(vec3 p, vec3 center, float r){ return length(p - center) - r; }
    float smin(float a, float b, float k) {
        // polynomial smooth min (smooth union)
        float h = clamp(0.5 + 0.5*(b - a)/k, 0.0, 1.0);
        return mix(b, a, h) - k*h*(1.0 - h);
    }

    // combined SDF for two spheres using smooth union with parameter k
    float map(vec3 p, vec3 c1, float r1, vec3 c2, float r2, float k) {
        float a = sdSphere(p, c1, r1);
        float b = sdSphere(p, c2, r2);
        return smin(a, b, k);
    }

    vec3 calcNormal(vec3 p, vec3 c1, float r1, vec3 c2, float r2, float k, float eps){
        // central differences on the combined map for accurate normal
        float dx = map(p + vec3(eps,0,0), c1,r1,c2,r2,k) - map(p - vec3(eps,0,0), c1,r1,c2,r2,k);
        float dy = map(p + vec3(0,eps,0), c1,r1,c2,r2,k) - map(p - vec3(0,eps,0), c1,r1,c2,r2,k);
        float dz = map(p + vec3(0,0,eps), c1,r1,c2,r2,k) - map(p - vec3(0,0,eps), c1,r1,c2,r2,k);
        return normalize(vec3(dx, dy, dz));
    }

    void main(){
        vec2 uv = TexCoords * 2.0 - 1.0;
        vec4 rayClip = vec4(uv, -1.0, 1.0);
        vec4 rayEye = invProj * rayClip; rayEye.z = -1.0; rayEye.w = 0.0;
        vec3 rayDir = normalize((invView * rayEye).xyz);
        vec3 ro = camPos;

        // animate two sphere centers
        vec3 c1 = vec3(-0.6 + sin(iTime)*0.4, 0.0, -2.5);
        vec3 c2 = vec3(0.6 + cos(iTime*1.3)*0.4, 0.0, -2.5);
        float r1 = 0.7; float r2 = 0.7;

        float t = 0.0; bool hit = false; vec3 p;
        for(int i=0;i<uSteps;i++){
            p = ro + t * rayDir;
            float d = map(p, c1, r1, c2, r2, uK);
            if(d < uEpsilon){ hit = true; break; }
            if(t > uMaxDist) break;
            t += max(d, 0.001) * 0.8;
        }

        vec3 col = vec3(0.02);
        if(hit){
            vec3 N = calcNormal(p,c1,r1,c2,r2,uK,uEpsilon*0.5);
            vec3 L = normalize(vec3(0.5,0.8,0.6));
            float diff = max(dot(N,L),0.0);
            float spec = pow(max(dot(reflect(-L,N), normalize(ro-p)),0.0), 32.0);
            col = vec3(0.3,0.6,0.9) * diff + vec3(1.0)*spec;
        }

        FragColor = vec4(col,1.0);
    }
    )";

    sdfShader.reset(new KEngine::Shader(vs, fs));

    quadMesh->SetDrawState(sdfShader, false, false);
    quadMesh->UseDelayRender() = false; // draw in forward pass

    Objects.push_back(quadMesh);
}
void SDFMix::OnUpdate(KEngine::TimeStep ts)
{
    mainCamera->Control(ts.GetTimeStep());
    // update uniforms for sdf shader
    if (sdfShader) {
        // set inverse projection and view
        glm::mat4 proj = mainCamera->GetProjMatrix();
        glm::mat4 view = mainCamera->GetViewMatrix();
        glm::mat4 invProj = glm::inverse(proj);
        glm::mat4 invView = glm::inverse(view);
        sdfShader->Bind();
        sdfShader->SetUniformMatrix4fv(invProj, "invProj");
        sdfShader->SetUniformMatrix4fv(invView, "invView");
        sdfShader->SetUniform3f(mainCamera->GetPosition(), "camPos");
        static float timeAcc = 0.0f;
        timeAcc += ts.GetTimeStep();
        sdfShader->SetUniform1f(timeAcc, "iTime");
        // raymarch parameters
        sdfShader->SetUniform1i(128, "uSteps");
        sdfShader->SetUniform1f(50.0f, "uMaxDist");
        sdfShader->SetUniform1f(0.001f, "uEpsilon");
        sdfShader->SetUniform1f(0.25f, "uK");
    }

}
void SDFMix::Destroy()
{

	pointLight0.reset();

	Objects.clear();
	pointLightList.clear();
	parallelLightList.clear();
}
