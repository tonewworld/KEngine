#include "ParaShadow.h"

ParaShadow::ParaShadow(std::string name) :name(name)
{
}

ParaShadow::~ParaShadow()
{
	Destroy();
}

void ParaShadow::Init()
{

	mainCamera = std::make_unique<KEngine::Camera>();
	projMatrix = glm::perspective(glm::radians(45.f), (float)
		KEngine::Application::s_Instance->GetWindow().GetWidth()
		/ KEngine::Application::s_Instance->GetWindow().GetHeight(),
		0.1f, 300.f);
	{
		char* vertexSrc = R"(
					#version 420 core
					layout(location=0) in vec3 v_Position;
					layout(location=1) in vec3 v_Normal;

					uniform mat4 model;
					layout(std140) uniform VPMatrix
					{
						mat4 view;
						mat4 proj;
					};
			
					out VS_OUT {
						vec3 normal;
						vec3 fragPos;
					} vs_out;
							

					void main()
					{
						gl_Position = proj * view * model * vec4(v_Position,1.0);
						vs_out.fragPos = vec3(model*vec4(v_Position,1.0));
						vs_out.normal = normalize(mat3(transpose(inverse(model))) * v_Normal);
						
					}
				)";
		
		char* fragmentSrc = R"(#version 420 core
					out vec4 FragColor;
					in VS_OUT {
						vec3 normal;
						vec3 fragPos;
					} fs_in;

					
					layout(std140) uniform MaterialUboData{
						vec3 Ambient;
						float _pad0;
						vec3 Diffuse;
						float _pad1;
						vec3 Specular;
						float _pad2;
						float Shininess;
						float _pad3[3];
					}material;


					struct ParallelLight{
						vec3 Direct;   float _pad0;
						vec3 Ambient;  float _pad1;
						vec3 Diffuse;  float _pad2;
						vec3 Specular; float _pad3;
						vec3 Color;	   float _pad4;
					};
					
					layout(std140) uniform ParallelLightUboData{
						ParallelLight parallelLightList[10];
						int parallelLightCount;
						int _pad1[3];
					};

					uniform vec3 viewPos;
					uniform sampler2D shadowMap;
					uniform mat4 lightSpaceMatrix;
					uniform bool useBlin;

					 float CalculateParallelLightShadow(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir) {
					
						vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
        
						projCoords = projCoords * 0.5 + 0.5;
        
						
						if(projCoords.x < 0.0 || projCoords.x > 1.0 || 
						   projCoords.y < 0.0 || projCoords.y > 1.0 ||
						   projCoords.z < 0.0 || projCoords.z > 1.0) {
							return 0.0; 
						}
						float closestDepth = texture(shadowMap, projCoords.xy).r;
        
						float currentDepth = projCoords.z;
        
						float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
        
						float shadow = 0.0;
						vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
						for(int x = -1; x <= 1; ++x)
						{
							for(int y = -1; y <= 1; ++y)
							{
								float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
								shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
							}    
						}
						shadow /= 9.0;
						
						
						return shadow;
					}
					vec3 CalculateParallelLight(){
						vec3 norm    = normalize(fs_in.normal);
						vec3 viewDir = normalize(viewPos - fs_in.fragPos);
						vec3 total   = vec3(0.0);          

						for (int i = 0; i < parallelLightCount; ++i) {
							vec3 lightDir = normalize(-parallelLightList[i].Direct);

							vec4 fragPosLightSpace = lightSpaceMatrix * vec4(fs_in.fragPos, 1.0);
							float shadow = CalculateParallelLightShadow(fragPosLightSpace, norm, lightDir);

							vec3 ambient  = parallelLightList[i].Color * parallelLightList[i].Ambient * material.Ambient;
							float diff    = max(dot(norm, lightDir), 0.0);
							vec3 diffuse  = parallelLightList[i].Color * parallelLightList[i].Diffuse * material.Diffuse * diff;

							vec3 halfwayDir = normalize(lightDir + viewDir);
							vec3 reflectDir = reflect(-lightDir,norm);

							float spec;
							if(useBlin)
								spec = pow(max(dot(norm,halfwayDir), 0.0), material.Shininess);
							else
								spec = pow(max(dot(viewDir,reflectDir),0.0),material.Shininess);
						
							vec3 specular   = parallelLightList[i].Color * parallelLightList[i].Specular * material.Specular * spec;

							total += ambient + (1.0 - shadow) * (diffuse + specular);
						}
						return total;
					}
					
					void main() {
						
						FragColor = vec4(CalculateParallelLight(), 1.0);
					}
				)";

		m_Shader.reset(new KEngine::Shader(vertexSrc, fragmentSrc));

	}

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

	float m_Vertices[] = {
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	};
	unsigned int m_Indices[]{
		0,1,2,
		3,4,5,
		6,7,8,
		9,10,11,
		12,13,14,
		15,16,17,
		18,19,20,
		21,22,23,
		24,25,26,
		27,28,29,
		30,31,32,
		33,34,35
	};

	KEngine::BufferLayout m_Layout = {
		{KEngine::ShaderDataType::Float3,"position"} ,
		{KEngine::ShaderDataType::Float3,"normal"}
	};
	m_Mesh.reset(new KEngine::Mesh(m_Vertices, sizeof(m_Vertices) / sizeof(float),
		m_Layout,
		m_Indices, sizeof(m_Indices) / sizeof(unsigned int),
		"m"));
	m_Mesh->SetMaterial({ glm::vec3(1.0f,0.5f,0.31f),
		glm::vec3(1.0f,0.5f,0.31f),
		glm::vec3(0.5f,0.5f,0.5f),
		32.0f
		});
	m_Mesh1.reset(new KEngine::Mesh(m_Vertices, sizeof(m_Vertices) / sizeof(float),
		m_Layout,
		m_Indices, sizeof(m_Indices) / sizeof(unsigned int),
		"m1"));
	m_Mesh1->SetMaterial({ glm::vec3(1.0f,0.5f,0.31f),
		glm::vec3(1.0f,0.5f,0.31f),
		glm::vec3(0.5f,0.5f,0.5f),
		32.0f
		});
	m_Mesh1->SetPosition(glm::vec3(0.0f, 0.0f, -5.0f));

	float l_Vertices[] = {
	-0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,
	 0.5f,  0.5f, -0.5f,
	 0.5f,  0.5f, -0.5f,
	-0.5f,  0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,

	-0.5f, -0.5f,  0.5f,
	 0.5f, -0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f,  0.5f,
	-0.5f, -0.5f,  0.5f,

	-0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,
	-0.5f, -0.5f,  0.5f,
	-0.5f,  0.5f,  0.5f,

	 0.5f,  0.5f,  0.5f,
	 0.5f,  0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,

	-0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f,  0.5f,
	 0.5f, -0.5f,  0.5f,
	-0.5f, -0.5f,  0.5f,
	-0.5f, -0.5f, -0.5f,

	-0.5f,  0.5f, -0.5f,
	 0.5f,  0.5f, -0.5f,
	 0.5f,  0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f, -0.5f
	};
	unsigned int l_Indices[]{
		0,1,2,
		3,4,5,
		6,7,8,
		9,10,11,
		12,13,14,
		15,16,17,
		18,19,20,
		21,22,23,
		24,25,26,
		27,28,29,
		30,31,32,
		33,34,35
	};

	KEngine::BufferLayout l_Layout = {
		{KEngine::ShaderDataType::Float3,"position"}
	};
	parallelLight0.reset(new KEngine::ParallelLight(l_Vertices, sizeof(l_Vertices) / sizeof(float),
		l_Layout,
		l_Indices, sizeof(l_Indices) / sizeof(unsigned int),
		"parallelLight0"));
	parallelLight0->SetLightAttributes({
		glm::vec3( 0.f, 0.f,-1.f),
		glm::vec3(0.2f,0.2f,0.2f),
		glm::vec3(0.5f,0.5f,0.5f),
		glm::vec3(1.0f,1.0f,1.0f),
		glm::vec3(1.0f,1.0f,1.0f)
		});
	parallelLight0->SetPosition(glm::vec3(0.0f, 0.0f, 5.0f));
	parallelLight0->SetScale(glm::vec3(0.2f));
	parallelLight0->SetIsLight(true);

	vpSL.clear();
	vpSL = {
		m_Shader,
		l_Shader
	};
	//一个shader的列表,为他们每一个绑定相同的VP矩阵
	for (auto& shader : vpSL)
	{
		shader->BindUniformBufferPoint("VPMatrix", 0);
	}
	
	m_Shader->BindUniformBufferPoint("MaterialUboData", 1);
	m_Shader->BindUniformBufferPoint("ParallelLightUboData", 2);
	
	//生成uniform缓冲对象
	matrixUBO.reset(KEngine::UniformBuffer::Create(2 * sizeof(glm::mat4),0));
	materialUBO.reset(KEngine::UniformBuffer::Create(sizeof(KEngine::MaterialUboData),1));
	parallelLightUBO.reset(KEngine::UniformBuffer::Create(11 * sizeof(KEngine::PointLightUboData), 2));


	parallelLightList.push_back(parallelLight0);

	Objects.push_back(m_Mesh);
	Objects.push_back(m_Mesh1);
	Objects.push_back(parallelLight0);

}
void ParaShadow::OnUpdate(KEngine::TimeStep ts)
{
	mainCamera->Control(ts.GetTimeStep());

	// 填充数据到uniform缓冲对象
	matrixUBO->AddVPMatrix(mainCamera->GetViewMatrix(), projMatrix, 0);

	// 设置着色器uniform
	m_Shader->SetUniform3f(mainCamera->GetPosition(), "viewPos");
	m_Shader->SetUniform1i(29, "shadowMap");
	// 填充UBO
	materialUBO->AddMaterial(KEngine::MaterialUboData{ m_Mesh->GetMaterial()});
	parallelLightUBO->AddParallelLight(parallelLightList);

	// 设置绘制状态
	parallelLight0->SetDrawState(l_Shader, true, false, 0);
	m_Mesh->SetDrawState(m_Shader, true, false, 1, GL_LESS, 1, 0xFF);
	m_Mesh1->SetDrawState(m_Shader, true, false, 1, GL_LESS, 1, 0xFF);

	
}
void ParaShadow::Destroy()
{
	m_Mesh.reset();
	m_Mesh1.reset();
	parallelLight0.reset();

	matrixUBO.reset();
	materialUBO.reset();
	parallelLightUBO.reset();

	Objects.clear();
	pointLightList.clear();
	parallelLightList.clear();
}
