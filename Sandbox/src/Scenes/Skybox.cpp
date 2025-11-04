#include "Skybox.h"

Skybox::Skybox(std::string name):name(name)
{
	
}

Skybox::~Skybox()
{
	
	
}


void Skybox::Init()
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

				uniform vec3 viewPos;
				
				//Reflect
				uniform samplerCube skybox;	

				void main()
				{
				
					//Reflect
					vec3 I = normalize(fs_in.fragPos - viewPos);
					vec3 R = reflect(I, fs_in.normal);
					FragColor = vec4(texture(skybox, R).rgb, 1.0);
				}
			)";

		m_Shader.reset(new KEngine::Shader(vertexSrc, fragmentSrc));

	}
	{
		char* vertexSrc = R"(
				#version 420 core
				layout(location=0) in vec3 v_Position;
				
				uniform mat4 model;
				layout(std140) uniform VPMatrix
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


	{
		char* vertexSrc = R"(
				#version 420 core
				layout (location = 0) in vec3 position;
				out vec3 TexCoords;

				uniform mat4 model;
				layout(std140) uniform VPMatrix
				{
					mat4 view;
					mat4 proj;
				};

				void main()
				{
					gl_Position = proj * mat4(mat3(view)) * model * vec4(position, 1.0);  
					TexCoords = position;
				}
				)";
		char* fragmentSrc = R"(
				#version 420 core
				in vec3 TexCoords;
				out vec4 color;

				uniform samplerCube skybox;

				void main()
				{
					color = texture(skybox, TexCoords);
				}
				)";
		sky_Shader.reset(new KEngine::Shader(vertexSrc, fragmentSrc));
		sky_Shader->Bind();
		sky_Shader->SetUniform1i(1, "skybox");
	}
	{
		char* vertexSrc = R"(
				#version 420 core
				layout(location=0) in vec3 v_Position;
				layout(location=1) in vec3 v_Normal;
				layout(location=2) in vec2 v_TexCoords;
				
				uniform mat4 model;
				layout(std140) uniform VPMatrix
				{
					mat4 view;
					mat4 proj;
				};


				out vec2 texCoords;

				void main()
				{
					gl_Position = proj * view * model * vec4(v_Position,1.0f);
					texCoords = v_TexCoords;
				}
			)";
		char* fragmentSrc = R"(
				#version 420 core

				in vec2 texCoords;
				out vec4 color;

				uniform sampler2D m_Texture;

				void main()
				{
			
					color =  texture(m_Texture, texCoords); 
				}

			)";
		backpack_Shader.reset(new KEngine::Shader(vertexSrc, fragmentSrc));
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

	std::vector<std::string> faces;
	faces.push_back("references\\skybox\\right.jpg");
	faces.push_back("references\\skybox\\left.jpg");
	faces.push_back("references\\skybox\\top.jpg");
	faces.push_back("references\\skybox\\bottom.jpg");
	faces.push_back("references\\skybox\\back.jpg");
	faces.push_back("references\\skybox\\front.jpg");

	textureCube.reset(KEngine::TextureCube::Create(faces));
	textureCube->SetTexSlot(1);
	

	float sky_Vertices[] = {
		//Positions          
	   -1.0f,  1.0f, -1.0f,
	   -1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
	   -1.0f,  1.0f, -1.0f,

	   -1.0f, -1.0f,  1.0f,
	   -1.0f, -1.0f, -1.0f,
	   -1.0f,  1.0f, -1.0f,
	   -1.0f,  1.0f, -1.0f,
	   -1.0f,  1.0f,  1.0f,
	   -1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

	   -1.0f, -1.0f,  1.0f,
	   -1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
	   -1.0f, -1.0f,  1.0f,

	   -1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
	   -1.0f,  1.0f,  1.0f,
	   -1.0f,  1.0f, -1.0f,

	   -1.0f, -1.0f, -1.0f,
	   -1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
	   -1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};
	unsigned int sky_Indices[]{
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

	KEngine::BufferLayout sky_Layout = {
		{KEngine::ShaderDataType::Float3,"position"}
	};
	sky_Mesh.reset(new KEngine::Mesh(sky_Vertices, sizeof(sky_Vertices) / sizeof(float),
		sky_Layout,
		sky_Indices, sizeof(sky_Indices) / sizeof(unsigned int),
		"skybox"));

	//backpack_Model.reset(new KEngine::Model("references\\backpack\\backpack.obj", "backpack"));

	shaderList.clear();
	shaderList = {
		m_Shader,
		l_Shader,
		//backpack_Shader,
		sky_Shader
	};
	//一个shader的列表,为他们每一个绑定相同的VP矩阵
	for (auto& shader : shaderList)
	{
		shader->BindUniformBufferPoint("VPMatrix", 0);
	}
	//生成uniform缓冲对象
	matrixUBO.reset(KEngine::UniformBuffer::Create(2 * sizeof(glm::mat4),0));


	//Objects.push_back(backpack_Model);
	Objects.push_back(l_Mesh);
	Objects.push_back(m_Mesh);
	Objects.push_back(sky_Mesh);

}
void Skybox::OnUpdate(KEngine::TimeStep ts) {
	mainCamera->Control(ts.GetTimeStep());

	//填充数据到uniform缓冲对象
	matrixUBO->AddVPMatrix(mainCamera->GetViewMatrix(), projMatrix, 0);

	m_Shader->SetUniform1i(1, "skybox");

	//天空盒
	sky_Mesh->SetScale(glm::vec3(50.0f));
	sky_Mesh->SetDrawState(textureCube, sky_Shader, true, true, 0);
	
	//光源
	l_Mesh->SetDrawState(nullptr, l_Shader, true, false, 0);
	//物体
	m_Mesh->SetDrawState(textureCube, m_Shader, true, false, 1, GL_LESS, 1, 0xFF);

	//背包
	//backpack_Model->SetDrawState(nullptr, backpack_Shader, true, true, 0, GL_ALWAYS, 1, 0xFF);

}
void Skybox::Destroy()
{
	m_Mesh.reset();
	l_Mesh.reset();
	sky_Mesh.reset();
	//backpack_Model.reset();

	textureCube.reset();
	matrixUBO.reset();

	Objects.clear();

}
