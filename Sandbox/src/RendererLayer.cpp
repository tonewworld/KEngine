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

		m_Shader.reset(new KEngine::Shader(vertexSrc, geometrySrc, fragmentSrc));

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
	{
		const char* pickVS = R"(
				#version 420 core
				layout(location=0) in vec3 v_Position;
				uniform mat4 model;
				layout(std140) uniform VPMatrix
				{
					mat4 view;
					mat4 proj;
				};

				void main(){ 
					gl_Position = proj * view * model * vec4(v_Position,1.0); 
				}
			)";

						const char* pickFS = R"(
				#version 420 core
				out vec4 OutColor;
				uniform vec3 pickColor;
				void main(){
					OutColor = vec4(pickColor,1.0);
				 }			
			)";

		pickShader.reset(new KEngine::Shader(pickVS, pickFS));
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
	glm::mat4 lightModel= glm::scale(glm::translate(glm::mat4(1.0f), lightPosition), glm::vec3(0.01f));
	l_Mesh->SetModelMatrix(lightModel);
	
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

	pickFBO.reset(KEngine::FrameBuffer::Create());
	pickTexture.reset(KEngine::Texture2D::Create());
	pickFBO->AddTexture(pickTexture->GetRendererID());
	pickRBO.reset(KEngine::RenderBuffer::Create());

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
		sky_Indices, sizeof(sky_Indices) / sizeof(unsigned int),
		"skybox"));
	
	backpack_Model.reset(new KEngine::Model("references\\backpack\\backpack.obj","backpack"));
	
	shaderList.clear();
	shaderList = {
		m_Shader,
		l_Shader,
		backpack_Shader,
		pickShader,
		sky_Shader
	};
	//一个shader的列表,为他们每一个绑定相同的VP矩阵
	for (auto& shader : shaderList)
	{
		shader->BindUniformBufferPoint("VPMatrix", 0);
	}
	//生成uniform缓冲对象
	matrixUBO.reset(KEngine::UniformBuffer::Create(2 * sizeof(glm::mat4)));


	Objects.push_back(backpack_Model);
	Objects.push_back(l_Mesh);
	Objects.push_back(m_Mesh);
	Objects.push_back(sky_Mesh);

}

void RendererLayer::OnAttach() {
	KEngine::Renderer::Init();

}
void RendererLayer::OnDetach()
{
	FBO.reset();
	RBO.reset();
	pickFBO.reset();
	pickRBO.reset();
}
void RendererLayer::OnUpdate(KEngine::TimeStep ts) {

	FBO->Bind();
	KEngine::Renderer::BeginScene();

	mainCamera->Control(ts.GetTimeStep());

	//填充数据到uniform缓冲对象
	matrixUBO->AddVPMatrix(mainCamera->GetViewMatrix(), projMatrix, 0);

	//天空盒
	sky_Mesh->SetModelMatrix(glm::scale(glm::mat4(1.0f), glm::vec3(100.0f)));
	sky_Mesh->SetDrawState(textureCube,sky_Shader, true, true, 0);
	//光源
	l_Mesh->SetDrawState(nullptr, l_Shader, true, false, 0);
	//物体
	m_Shader->SetUniform3f(mainCamera->GetPosition(), "viewPos");
	m_Mesh->SetDrawState(nullptr, m_Shader, true, false, 1, GL_LESS, 1, 0xFF);
	//背包
	backpack_Model->SetDrawState(nullptr, backpack_Shader, true, true, 0, GL_ALWAYS, 1, 0xFF);

	for (const auto& obj : Objects)
	{
		obj->shader->SetUniformMatrix4fv(obj->GetModelMatrix(), "model");
		obj->Draw(obj->shader);
	}
	FBO->Unbind();

	PickWithColor();

	quad_Mesh->SetDrawState(quad_Texture, screenShader, false, false);
	quad_Mesh->Draw(screenShader);
	


	KEngine::Renderer::EndScene();
}

void RendererLayer::OnEvent(KEngine::Event& event)
{
}

void RendererLayer::ImGuiRender()
{
	
}

void RendererLayer::PickWithColor()
{
	float mouseX = 0.f,mouseY = 0.f;
	
	if (KEngine::Camera::CheckLeftMouseButtonPress())
	{
		std::pair<float, float> mousePosition = KEngine::Input::GetMousePosition();
		mouseX = mousePosition.first;
		mouseY = mousePosition.second;

		auto EncodeIDToColor = [](int id)->glm::vec3 {
			unsigned char r = id & 0xFF;
			unsigned char g = (id >> 8) & 0xFF;
			unsigned char b = (id >> 16) & 0xFF;
			return glm::vec3(r / 255.0f, g / 255.0f, b / 255.0f);
			};
		auto DecodeColorToID = [](unsigned char r, unsigned char g, unsigned char b)->int {
			return r + (g << 8) + (b << 16);
			};

		// 离屏渲染到 FBO 的颜色缓冲
		pickFBO->Bind();
		KEngine::Renderer::ColorPickBegin();

		// 渲染每个对象为其 ID color（只写颜色）
		for (const auto& obj : Objects)
		{
			int id = obj->GetID();            // 需要存在
			glm::vec3 color = EncodeIDToColor(id);
			glm::mat4 model = obj->GetModelMatrix(); // 需要存在

			pickShader->SetUniformMatrix4fv(model, "model");
			pickShader->SetUniform3f(color, "pickColor");

			// 需要对象能以 shader 绘制自己（或提供 mesh/model 访问）
			KEngine::Renderer::Submit(pickShader, obj);
		}

		// 读取像素（窗口坐标到 GL 底部原点）
		int width = KEngine::Application::s_Instance->GetWindow().GetWidth();
		int height = KEngine::Application::s_Instance->GetWindow().GetHeight();
		int rx = static_cast<int>(mouseX);
		int ry = height - 1 - static_cast<int>(mouseY);

		auto pixel = KEngine::Renderer::ReadPixel(rx, ry);
		std::cout << "Read pixel at (" << rx << ", " << ry << ") -> RGB: "
			<< (int)pixel[0] << ", " << (int)pixel[1] << ", " << (int)pixel[2] << std::endl;

		pickFBO->Unbind();

		int pickedID = DecodeColorToID(pixel[0], pixel[1], pixel[2]);
		if (pickedID == 0) {
			// 背景或未命中
			std::cout << "Pick: nothing\n";
			return;
		}

		// 在 Objects 中查找 pickedID（根据你的容器方式调整）
		for (const auto& obj : Objects) {
			if (obj->GetID() == pickedID) {
				std::cout << "Pick: ID=" << pickedID << " Name=" << obj->GetName() << "\n";
				// 在这里把属性返回、触发事件或填充 UI
				return;
			}
		}
		std::cout << "Pick: unknown ID=" << pickedID << "\n";
	}
	return;
}