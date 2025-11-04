#include "NormalMapping.h"

NormalMapping::NormalMapping(std::string name) :name(name)
{
}

NormalMapping::~NormalMapping()
{
	Destroy();
}

void NormalMapping::Init()
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
					layout(location = 2) in vec2 v_TexCoord; 
					layout(location = 3) in vec3 v_Tangent;  

					uniform mat4 model;
					layout(std140) uniform VPMatrix
					{
						mat4 view;
						mat4 proj;
					
					};
					
					out VS_OUT {
						vec3 fragPos;
						vec2 texCoord;
						mat3 TBN;
					} vs_out;
							

					void main()
					{
						gl_Position = proj * view * model * vec4(v_Position,1.0);

						vec3 T = normalize(mat3(model) * v_Tangent);
						vec3 N = normalize(mat3(transpose(inverse(model))) * v_Normal);
						T = normalize(T - dot(T, N) * N);
						vec3 B = cross(N, T);
						
						vs_out.fragPos = vec3(model*vec4(v_Position,1.0));
						vs_out.texCoord=v_TexCoord;
						vs_out.TBN=mat3(T,B,N);
					}
				)";

		char* fragmentSrc = R"(#version 420 core
					out vec4 FragColor;
					in VS_OUT {
						vec3 fragPos;
						vec2 texCoord;
						mat3 TBN;
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
					uniform sampler2D u_DiffuseMap;  
					uniform sampler2D u_NormalMap;    

					vec3 CalculateParallelLight(){
						vec3 baseColor = texture(u_DiffuseMap, fs_in.texCoord).rgb;
						vec3 normalTangent = texture(u_NormalMap, fs_in.texCoord).rgb * 2.0 - 1.0;
						vec3 norm = normalize(fs_in.TBN * normalTangent); 

						vec3 viewDir = normalize(viewPos - fs_in.fragPos);
						vec3 total   = vec3(0.0);          

						for (int i = 0; i < parallelLightCount; ++i) {
							vec3 lightDir = normalize(-parallelLightList[i].Direct);

							vec3 ambient  = parallelLightList[i].Color * parallelLightList[i].Ambient * baseColor;
							float diff    = max(dot(norm, lightDir), 0.0);
							vec3 diffuse  = parallelLightList[i].Color * parallelLightList[i].Diffuse * baseColor * diff;

							vec3 halfwayDir = normalize(lightDir + viewDir);
							float spec      = pow(max(dot(norm,halfwayDir), 0.0), material.Shininess);
							vec3 specular   = parallelLightList[i].Color * parallelLightList[i].Specular * baseColor * spec;

							 total += ambient + diffuse + specular;
						}
						return total;
					}
					
					void main() {
						
						FragColor =vec4(CalculateParallelLight(),1.0f);
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
						color = vec4(0.5f);
					}

				)";
		l_Shader.reset(new KEngine::Shader(vertexSrc, fragmentSrc));
	}


	float m_Vertices[] = {
		// px, py, pz,  nx, ny, nz,  u, v,  tx, ty, tz
		//位置                 //法向量              //uv          //切线
		 0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   1.0f, 0.0f,   1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   0.0f, 0.0f,   1.0f, 0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   0.0f, 1.0f,   1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   1.0f, 0.0f,   1.0f, 0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   0.0f, 1.0f,   1.0f, 0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   1.0f, 1.0f,   1.0f, 0.0f, 0.0f,

		 
		 -0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   1.0f, 0.0f,  -1.0f, 0.0f, 0.0f,
		  0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   0.0f, 0.0f,  -1.0f, 0.0f, 0.0f,
		  0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   0.0f, 1.0f,  -1.0f, 0.0f, 0.0f,
		 -0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   1.0f, 0.0f,  -1.0f, 0.0f, 0.0f,
		  0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   0.0f, 1.0f,  -1.0f, 0.0f, 0.0f,
		 -0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   1.0f, 1.0f,  -1.0f, 0.0f, 0.0f,


		  0.5f, -0.5f, -0.5f,   1.0f,  0.0f,  0.0f,   1.0f, 0.0f,   0.0f, 0.0f, 1.0f,
		  0.5f, -0.5f,  0.5f,   1.0f,  0.0f,  0.0f,   0.0f, 0.0f,   0.0f, 0.0f, 1.0f,
		  0.5f,  0.5f,  0.5f,   1.0f,  0.0f,  0.0f,   0.0f, 1.0f,   0.0f, 0.0f, 1.0f,
		  0.5f, -0.5f, -0.5f,   1.0f,  0.0f,  0.0f,   1.0f, 0.0f,   0.0f, 0.0f, 1.0f,
		  0.5f,  0.5f,  0.5f,   1.0f,  0.0f,  0.0f,   0.0f, 1.0f,   0.0f, 0.0f, 1.0f,
		  0.5f,  0.5f, -0.5f,   1.0f,  0.0f,  0.0f,   1.0f, 1.0f,   0.0f, 0.0f, 1.0f,


		  -0.5f, -0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,   0.0f, 0.0f, -1.0f,
		  -0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,   0.0f, 0.0f, -1.0f,
		  -0.5f,  0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,   0.0f, 1.0f,   0.0f, 0.0f, -1.0f,
		  -0.5f, -0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,   0.0f, 0.0f, -1.0f,
		  -0.5f,  0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,   0.0f, 1.0f,   0.0f, 0.0f, -1.0f,
		  -0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,   1.0f, 1.0f,   0.0f, 0.0f, -1.0f,


		  -0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,   0.0f, 0.0f,   1.0f, 0.0f, 0.0f,
		   0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,   1.0f, 0.0f,   1.0f, 0.0f, 0.0f,
		   0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,   1.0f, 1.0f,   1.0f, 0.0f, 0.0f,
		  -0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,   0.0f, 0.0f,   1.0f, 0.0f, 0.0f,
		   0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,   1.0f, 1.0f,   1.0f, 0.0f, 0.0f,
		  -0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,   0.0f, 1.0f,   1.0f, 0.0f, 0.0f,


		  -0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,   0.0f, 1.0f,  -1.0f, 0.0f, 0.0f,
		   0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,   1.0f, 1.0f,  -1.0f, 0.0f, 0.0f,
		   0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,   1.0f, 0.0f,  -1.0f, 0.0f, 0.0f,
		  -0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,   0.0f, 1.0f,  -1.0f, 0.0f, 0.0f,
		   0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,   1.0f, 0.0f,  -1.0f, 0.0f, 0.0f,
		  -0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,   0.0f, 0.0f,  -1.0f, 0.0f, 0.0f
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
		{KEngine::ShaderDataType::Float3,"normal"},
		{KEngine::ShaderDataType::Float2, "v_TexCoord"},
		{KEngine::ShaderDataType::Float3, "v_Tangent"}  
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
	m_DiffuseMap.reset(KEngine::Texture2D::Create("references/normalMapping/brickwall.jpg"));
	m_DiffuseMap->SetTexSlot(1);
	m_NormalMap.reset(KEngine::Texture2D::Create("references/normalMapping/brickwall_normal.jpg"));
	m_NormalMap->SetTexSlot(2);
	m_Mesh->diffuseMap = m_DiffuseMap;
	m_Mesh->normalMap = m_NormalMap;

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
		glm::vec3(0.f, 0.f,-1.f),
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
	matrixUBO.reset(KEngine::UniformBuffer::Create(2 * sizeof(glm::mat4), 0));
	materialUBO.reset(KEngine::UniformBuffer::Create(sizeof(KEngine::MaterialUboData), 1));
	parallelLightUBO.reset(KEngine::UniformBuffer::Create(11 * sizeof(KEngine::PointLightUboData), 2));

	parallelLightList.push_back(parallelLight0);

	Objects.push_back(m_Mesh);
	Objects.push_back(parallelLight0);

}
void NormalMapping::OnUpdate(KEngine::TimeStep ts)
{
	mainCamera->Control(ts.GetTimeStep());

	// 填充数据到uniform缓冲对象
	matrixUBO->AddVPMatrix(mainCamera->GetViewMatrix(), projMatrix, 0);
	// 设置着色器uniform
	m_Shader->SetUniform3f(mainCamera->GetPosition(), "viewPos");
	m_Shader->SetUniform1i(1, "u_DiffuseMap"); 
	m_Shader->SetUniform1i(2, "u_NormalMap");
	// 填充UBO
	materialUBO->AddMaterial(KEngine::MaterialUboData{ m_Mesh->GetMaterial() });
	parallelLightUBO->AddParallelLight(parallelLightList);

	parallelLight0->SetDrawState(nullptr, l_Shader, true, false, 0);
	m_Mesh->SetDrawState(nullptr, m_Shader, true, false, 1, GL_LESS, 1, 0xFF);

}
void NormalMapping::Destroy()
{
	m_Mesh.reset();
	parallelLight0.reset();

	matrixUBO.reset();
	materialUBO.reset();
	parallelLightUBO.reset();

	Objects.clear();
	pointLightList.clear();
	parallelLightList.clear();
}
