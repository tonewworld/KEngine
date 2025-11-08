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
		m_Shader->Bind();
		m_Shader->SetUniform1i(1, "skybox");
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
	m_Mesh->SetPosition(glm::vec3(1, 0, 0));
	m_Mesh->UseDelayRender() = false;
	m_Mesh->SetDrawState(m_Shader, true, false, 1, GL_LESS, 1, 0xFF);
	
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
	sky_Mesh->UseDelayRender() = false;
	sky_Mesh->SetDrawState(sky_Shader, true, true, 0);
	sky_Mesh->SetScale(glm::vec3(50.0f));

	backpack_Model.reset(new KEngine::Model("references\\backpack\\backpack.obj", "backpack"));
	backpack_Model->SetPosition(glm::vec3(-3, 0, 0));
	backpack_Model->UseDelayRender() = false;
	backpack_Model->SetDrawState(backpack_Shader, true, true, 0, GL_ALWAYS, 1, 0xFF);

	std::vector<std::string> faces;
	faces.push_back("references\\skybox\\right.jpg");
	faces.push_back("references\\skybox\\left.jpg");
	faces.push_back("references\\skybox\\top.jpg");
	faces.push_back("references\\skybox\\bottom.jpg");
	faces.push_back("references\\skybox\\back.jpg");
	faces.push_back("references\\skybox\\front.jpg");

	textureCube.reset(KEngine::TextureCube::Create(faces));
	textureCube->SetTexSlot(1);

	m_Mesh->AddTexture(textureCube);
	sky_Mesh->AddTexture(textureCube);


	Objects.push_back(sky_Mesh);
	Objects.push_back(backpack_Model);
	Objects.push_back(m_Mesh);

}
void Skybox::OnUpdate(KEngine::TimeStep ts) {
	mainCamera->Control(ts.GetTimeStep());

	

}
void Skybox::Destroy()
{
	m_Mesh.reset();
	sky_Mesh.reset();
	backpack_Model.reset();

	textureCube.reset();


	Objects.clear();

}
