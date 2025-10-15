#include "RendererLayer.h"


RendererLayer::RendererLayer() :Layer("Renderer") {
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
					mat4 VP;
				};

				out VS_OUT {
					vec3 normal;
					vec3 fragPos;
				} vs_out;
								

				void main()
				{
					gl_Position = VP * model * vec4(v_Position,1.0);
					vs_out.fragPos = vec3(model*vec4(v_Position,1.0));
					vs_out.normal = normalize(mat3(transpose(inverse(model))) * v_Normal);
				}
			)";
		char* geometrySrc = R"(
				#version 420 core
				layout (triangles) in;
				layout (triangle_strip, max_vertices = 9) out;

				in VS_OUT {
					vec3 normal;
					vec3 fragPos;
				} gs_in[];
				
				out GS_OUT {
					vec3 normal;
					vec3 fragPos;
				} gs_out;

				//没用到
				/*const float MAGNITUDE = 0.2f;

				void GenerateLine(int index)
				{
					gl_Position = gl_in[index].gl_Position;
					EmitVertex();
					gl_Position = gl_in[index].gl_Position + vec4(gs_in[index].normal, 0.0f) * MAGNITUDE;
					EmitVertex();
					EndPrimitive();
				}*/

				void main()
				{
					//GenerateLine(0); // First vertex normal
					//GenerateLine(1); // Second vertex normal
					//GenerateLine(2); // Third vertex normal
					 for (int i = 0; i < 3; ++i) {
						gl_Position = gl_in[i].gl_Position;
						gs_out.normal = gs_in[i].normal;
						gs_out.fragPos = gs_in[i].fragPos;
						EmitVertex();
					}
					EndPrimitive();
				}
			)";
		char* fragmentSrc = R"(#version 420 core
				out vec4 FragColor;
				in GS_OUT {
					vec3 normal;
					vec3 fragPos;
				} fs_in;

				uniform vec3 lightPos;
				uniform vec3 viewPos;
				uniform vec3 lightColor;
				uniform vec3 objectColor;
				
				//Reflect
				uniform samplerCube skybox;	

				void main()
				{
					// Ambient
					float ambientStrength = 0.1f;
					vec3 ambient = ambientStrength * lightColor;
  	
					// Diffuse 
					vec3 lightDir = normalize(lightPos - fs_in.fragPos);
					float diff = max(dot(fs_in.normal, lightDir), 0.0);
					vec3 diffuse = diff * lightColor;
    
					// Specular
					float specularStrength = 0.5f;
					vec3 viewDir = normalize(viewPos - fs_in.fragPos);
					vec3 reflectDir = reflect(-lightDir, fs_in.normal);  
					float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
					vec3 specular = specularStrength * spec * lightColor;  
        
					vec3 result = (ambient+diffuse+specular) * objectColor;

					//Reflect
					vec3 I = normalize(fs_in.fragPos - viewPos);
					vec3 R = reflect(I, fs_in.normal);
					FragColor = vec4(texture(skybox, R).rgb, 1.0);
				}
			)";

		m_Shader.reset(new KEngine::Shader(vertexSrc, geometrySrc, fragmentSrc));

	}
	{
		char* vertexSrc = R"(
				#version 420 core
				layout(location=0) in vec3 v_Position;
				
				uniform mat4 model;
				layout(std140) uniform VPMatrix
				{
					mat4 VP;
				};

				void main()
				{
					gl_Position = VP * model * vec4(v_Position,1.0);
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
				layout(location=0) in vec3 v_Position;
				layout(location=1) in vec3 v_Normal;

				
				uniform mat4 model;
				layout(std140) uniform VPMatrix
				{
					mat4 VP;
				};
				void main()
				{
					gl_Position = VP * model * vec4(v_Position,1.0);
				}
			)";
		char* fragmentSrc = R"(#version 420 core
				out vec4 FragColor;
				
				void main()
				{
					FragColor = vec4(0.7f);
				}
			)";

		s_Shader.reset(new KEngine::Shader(vertexSrc, fragmentSrc));

	}

	{
		char* vertexSrc = R"(
				#version 420 core
				layout (location = 0) in vec2 position;
				layout (location = 1) in vec2 texCoords;

				out vec2 TexCoords;

				void main()
				{
					gl_Position = vec4(position.x, position.y, 0.0f, 1.0f);
					TexCoords = texCoords;
				}
				)";
		char* fragmentSrc = R"(
				#version 420 core
				in vec2 TexCoords;
				out vec4 color;

				uniform sampler2D screenTexture;

				void main()
				{
					color =vec4(vec3(texture(screenTexture,TexCoords)),1.f);
				}
				)";

		screenShader.reset(new KEngine::Shader(vertexSrc, fragmentSrc));
	}
	{
		char* vertexSrc = R"(
				#version 420 core
				layout (location = 0) in vec3 position;
				out vec3 TexCoords;

				uniform mat4 model;
				uniform mat4 VP;
				
				void main()
				{
					gl_Position =  VP * model * vec4(position, 1.0);  
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
					mat4 VP;
				};

				out vec2 texCoords;

				void main()
				{
					gl_Position = VP * model * vec4(v_Position,1.0f);
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
		m_Indices, sizeof(m_Indices) / sizeof(unsigned int)));


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
		l_Indices, sizeof(l_Indices) / sizeof(unsigned int)));


	float quad_Vertices[] = {   // Vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
		// Positions   // TexCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};
	unsigned int quadIndexes[] = {
		0,1,2,
		3,4,5
	};
	KEngine::BufferLayout quad_Layout = {
		{KEngine::ShaderDataType::Float2,"position"} ,
		{KEngine::ShaderDataType::Float2,"texCoords"}
	};
	quad_Mesh.reset(new KEngine::Mesh(quad_Vertices, sizeof(quad_Vertices) / sizeof(float),
		quad_Layout,
		quadIndexes, sizeof(quadIndexes) / sizeof(unsigned int)));


	FBO.reset(KEngine::FrameBuffer::Create());

	quad_Texture.reset(KEngine::Texture2D::Create());
	FBO->AddTexture(quad_Texture->GetRendererID());
	RBO.reset(KEngine::RenderBuffer::Create());

	std::vector<std::string> faces;
	faces.push_back("references\\skybox\\right.jpg");
	faces.push_back("references\\skybox\\left.jpg");
	faces.push_back("references\\skybox\\top.jpg");
	faces.push_back("references\\skybox\\bottom.jpg");
	faces.push_back("references\\skybox\\back.jpg");
	faces.push_back("references\\skybox\\front.jpg");

	textureCube.reset(KEngine::TextureCube::Create(faces));



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
		sky_Indices, sizeof(sky_Indices) / sizeof(unsigned int)));

	//backpack_Model.reset(new KEngine::Model("references\\backpack\\backpack.obj"));

	shaderList.clear();
	shaderList = {
		m_Shader,
		l_Shader,
		s_Shader,
		backpack_Shader
	};
	//一个shader的列表,为他们每一个绑定相同的VP矩阵
	for (auto& shader : shaderList)
	{
		shader->BindUniformBufferPoint("VPMatrix", 0);
	}
	//生成uniform缓冲对象
	matrixUBO.reset(KEngine::UniformBuffer::Create(2 * sizeof(glm::mat4)));

}

void RendererLayer::OnAttach() {
	KEngine::Renderer::Init();

}
void RendererLayer::OnUpdate(KEngine::TimeStep ts) {

	FBO->Bind();
	KEngine::Renderer::BeginScene();

	mainCamera->Control(ts.GetTimeStep());

	//填充数据到uniform缓冲对象
	//这个函数感觉得改成一个模板函数？
	matrixUBO->AddUniformData(CalculateVP(mainCamera->GetViewMatrix(), projMatrix), 0);

	//天空盒
	textureCube->Bind();
	sky_Shader->SetUniformMatrix4fv(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f)), "model");
	sky_Shader->SetUniformMatrix4fv(CalculateVP(glm::mat4(glm::mat3(mainCamera->GetViewMatrix())), projMatrix), "VP");
	KEngine::Renderer::SetDepthOpenOrClose(false);
	KEngine::Renderer::SetStencilMask(0);
	KEngine::Renderer::Submit(sky_Shader, sky_Mesh);
	KEngine::Renderer::SetDepthOpenOrClose(true);

	//光源
	l_Shader->SetUniformMatrix4fv(glm::scale(glm::translate(glm::mat4(1.0f), lightPosition), glm::vec3(0.01f)), "model");
	KEngine::Renderer::SetStencilMask(0);
	KEngine::Renderer::Submit(l_Shader, l_Mesh);

	//物体
	textureCube->Bind();//Reflect
	m_Shader->SetUniformMatrix4fv(glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f)), glm::vec3(0.3f)), "model");
	m_Shader->SetUniform3f({ 1.0f,0.5f,0.31f }, "objectColor");
	m_Shader->SetUniform3f({ 1.0f,1.0f,1.0f }, "lightColor");
	m_Shader->SetUniform3f(lightPosition, "lightPos");
	m_Shader->SetUniform3f(mainCamera->GetPosition(), "viewPos");
	KEngine::Renderer::SetStencilFunc(GL_ALWAYS, 1, 0xFF);
	KEngine::Renderer::SetStencilMask(0xFF);
	//KEngine::Renderer::Submit(m_Shader, m_Mesh);

	//边框
	s_Shader->SetUniformMatrix4fv(glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f)), glm::vec3(0.32f)), "model");
	KEngine::Renderer::SetStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	KEngine::Renderer::SetStencilMask(0x00);
	KEngine::Renderer::SetDepthOpenOrClose(false);
	//KEngine::Renderer::Submit(s_Shader, m_Mesh);

	//背包
	backpack_Shader->SetUniformMatrix4fv(glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f)), glm::vec3(0.3f)), "model");
	KEngine::Renderer::SetStencilMask(0);
	KEngine::Renderer::SetDepthOpenOrClose(true);
	//KEngine::Renderer::Submit(backpack_Shader, backpack_Model);

	PickWithDepth(400.f,500.f);

	FBO->Unbind();
	KEngine::Renderer::SetDepthOpenOrClose(false);
	KEngine::Renderer::SetStencilOpenOrClose(false);

	quad_Texture->Bind();
	KEngine::Renderer::Submit(screenShader, quad_Mesh);
	
	KEngine::Renderer::EndScene();
}

void RendererLayer::OnEvent(KEngine::Event& event)
{
}

void RendererLayer::ImGuiRender()
{
	
}

void RendererLayer::PickWithDepth(float mouseX, float mouseY)
{
	// 简单颜色编码辅助
	auto EncodeIDToColor = [](int id)->glm::vec3 {
		unsigned char r = id & 0xFF;
		unsigned char g = (id >> 8) & 0xFF;
		unsigned char b = (id >> 16) & 0xFF;
		return glm::vec3(r / 255.0f, g / 255.0f, b / 255.0f);
		};

	auto DecodeColorToID = [](unsigned char r, unsigned char g, unsigned char b)->int {
		return r + (g << 8) + (b << 16);
		};

	// 创建一个临时 pick shader（与其它 shader 保持相同的 VP uniform block）
	char* pickVertex = R"(
		#version 420 core
		layout(location=0) in vec3 v_Position;
		uniform mat4 model;
		layout(std140) uniform VPMatrix
		{
			mat4 VP;
		};
		void main()
		{
			gl_Position = VP * model * vec4(v_Position,1.0);
		}
	)";
	char* pickFragment = R"(
		#version 420 core
		out vec4 FragColor;
		uniform vec3 pickColor;
		void main()
		{
			FragColor = vec4(pickColor, 1.0);
		}
	)";

	// 创建 shader（开销可接受：只在点击时创建一次也可以缓存）
	std::shared_ptr<KEngine::Shader> pickShader(new KEngine::Shader(pickVertex, pickFragment));
	// 确保它与其它 shader 使用同一个 VP UBO binding point（你的构造函数里把其他 shader 绑定到 0）
	pickShader->BindUniformBufferPoint("VPMatrix", 0);

	// 为每个可拾取对象分配ID（不要用0，0表示空背景）
	const int ID_MainMesh = 1;
	const int ID_LMesh = 2;
	const int ID_Backpack = 3; // 如果你有 backpack_Model，可设置并渲染

	std::unordered_map<int, std::string> idToName = {
		{ID_MainMesh, "MainObject"},
		{ID_LMesh, "LightCube"},
		{ID_Backpack, "BackpackModel"}
	};

	// 离屏渲染到已有 FBO（确保它有颜色附件）
	FBO->Bind();
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 渲染每个对象到颜色缓冲（只写颜色），使用唯一颜色表示 ID
	// Main mesh
	{
		glm::mat4 model = glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f)), glm::vec3(0.3f));
		pickShader->SetUniformMatrix4fv(model, "model");
		glm::vec3 c = EncodeIDToColor(ID_MainMesh);
		pickShader->SetUniform3f(c, "pickColor");
		KEngine::Renderer::Submit(pickShader, m_Mesh);
	}
	// Light cube
	{
		glm::mat4 model = glm::scale(glm::translate(glm::mat4(1.0f), lightPosition), glm::vec3(0.01f));
		pickShader->SetUniformMatrix4fv(model, "model");
		glm::vec3 c = EncodeIDToColor(ID_LMesh);
		pickShader->SetUniform3f(c, "pickColor");
		KEngine::Renderer::Submit(pickShader, l_Mesh);
	}
	// Backpack（如果存在并已加载）
	if (backpack_Model)
	{
		glm::mat4 model = glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f)), glm::vec3(0.3f));
		pickShader->SetUniformMatrix4fv(model, "model");
		glm::vec3 c = EncodeIDToColor(ID_Backpack);
		pickShader->SetUniform3f(c, "pickColor");
		KEngine::Renderer::Submit(pickShader, backpack_Model); // 假定 Submit 支持 Model
	}

	// 读取像素（坐标系：把鼠标 Y 翻转为 OpenGL 底部为原点）
	int width = KEngine::Application::s_Instance->GetWindow().GetWidth();
	int height = KEngine::Application::s_Instance->GetWindow().GetHeight();
	int readX = static_cast<int>(mouseX);
	int readY = height - 1 - static_cast<int>(mouseY);

	unsigned char pixel[4] = { 0,0,0,0 };
	glReadPixels(readX, readY, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixel);

	FBO->Unbind();

	int pickedID = DecodeColorToID(pixel[0], pixel[1], pixel[2]);
	if (pickedID == 0) {
		std::cout << "Pick: nothing\n";
		return;
	}

	auto it = idToName.find(pickedID);
	if (it != idToName.end()) {
		std::cout << "Pick: ID=" << pickedID << " Name=" << it->second << "\n";
		// TODO: 在这里把属性返回或发事件，或者查询你对象管理器拿到更详细属性并显示UI
	}
	else {
		std::cout << "Pick: unknown ID=" << pickedID << "\n";
	}
}