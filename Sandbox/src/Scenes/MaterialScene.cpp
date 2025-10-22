#include "MaterialScene.h"

MaterialScene::MaterialScene(std::string name) :name(name)
{
}

MaterialScene::~MaterialScene()
{
}

void MaterialScene::Init()
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

					layout(std140) uniform Material{
						vec3 ambient;
						vec3 diffuse;
						vec3 specular;
						float shininess;
					};
					uniform vec3 lightPos;	
					uniform vec3 viewPos;

					const vec3 lightColor = vec3(1.0f,1.0f,1.0f);

					void main()
					{
						FragColor = vec4(1.0f);
				
						// 环境光
						vec3 ambient = lightColor * Material.ambient;

						// 漫反射 
						vec3 norm = normalize(fs_in.normal);
						vec3 lightDir = normalize(lightPos - fs_in.fragPos);
						float diff = max(dot(norm, lightDir), 0.0);
						vec3 diffuse = lightColor * (diff * Material.diffuse);

						// 镜面光
						vec3 viewDir = normalize(viewPos - fs_in.fragPos);
						vec3 reflectDir = reflect(-lightDir, norm);  
						float spec = pow(max(dot(viewDir, reflectDir), 0.0), Material.shininess);
						vec3 specular = lightColor * (spec * Material.specular);  

						vec3 result = ambient + diffuse + specular;
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
						color = vec4(1.0f); //设置四维向量的所有元素为 1.0f
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
	l_Mesh.reset(new KEngine::Mesh(l_Vertices, sizeof(l_Vertices) / sizeof(float),
		l_Layout,
		l_Indices, sizeof(l_Indices) / sizeof(unsigned int),
		"light"));
	glm::mat4 lightModel = glm::scale(glm::translate(glm::mat4(1.0f), lightPosition), glm::vec3(0.01f));
	l_Mesh->SetModelMatrix(lightModel);

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
	
	m_Shader->BindUniformBufferPoint("Material", 1);
	
	//生成uniform缓冲对象
	matrixUBO.reset(KEngine::UniformBuffer::Create(2 * sizeof(glm::mat4),0));
	materialUBO.reset(KEngine::UniformBuffer::Create(4 * sizeof(glm::vec4),1));
	
	Objects.push_back(l_Mesh);
	Objects.push_back(m_Mesh);
}
void MaterialScene::OnUpdate(KEngine::TimeStep ts)
{
	mainCamera->Control(ts.GetTimeStep());
	//填充数据到uniform缓冲对象
	matrixUBO->AddVPMatrix(mainCamera->GetViewMatrix(), projMatrix, 0);

	//光源
	l_Mesh->SetDrawState(nullptr, l_Shader, true, false, 0);
	//物体
	m_Shader->SetUniform3f(lightPosition,"lightPos");
	m_Shader->SetUniform3f(mainCamera->GetPosition(), "viewPos");
	materialUBO->AddVec3(m_Mesh->GetMaterial().ambient, 0);
	materialUBO->AddVec3(m_Mesh->GetMaterial().diffuse, sizeof(glm::vec4));
	materialUBO->AddVec3(m_Mesh->GetMaterial().specular, sizeof(glm::vec4) * 2);
	materialUBO->AddFloat(m_Mesh->GetMaterial().shininess, sizeof(glm::vec4) * 3);
	m_Mesh->SetDrawState(nullptr, m_Shader, true, false, 1, GL_LESS, 1, 0xFF);

}
void MaterialScene::Destroy()
{
	m_Mesh.reset();
	l_Mesh.reset();
	matrixUBO.reset();
	Objects.clear();
}
