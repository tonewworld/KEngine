#include<KEngine.h>
#include "imgui.h"
#include "glad/glad.h"
class ExampleLayer : public KEngine::Layer {
	public:
	ExampleLayer():Layer("Example") {
		mainCamera = std::make_unique<KEngine::Camera>();
		projMatrix = glm::perspective(glm::radians(45.f),(float)
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

				uniform mat4 model;
				layout(std140) uniform VPMatrix
				{
					mat4 VP;
				};
				void main()
				{
					gl_Position = VP * model * vec4(v_Position,1.0f);
					gl_PointSize = 0.5f;
				}
			)";
			char* fragmentSrc = R"(
				#version 420 core
				out vec4 FragColor;
				void main()
				{
					FragColor=vec4(1.0f);
				}
			)";
			pointShader.reset(new KEngine::Shader(vertexSrc, fragmentSrc));
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
		m_Mesh.reset(new KEngine::Mesh(m_Vertices,sizeof(m_Vertices)/m_Layout.GetStride(),
			m_Layout,
			m_Indices,sizeof(m_Indices)/ sizeof(unsigned int)));


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
		l_Mesh.reset(new KEngine::Mesh(l_Vertices, sizeof(l_Vertices) / l_Layout.GetStride() ,
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
		quad_Mesh.reset(new KEngine::Mesh(quad_Vertices, sizeof(quad_Vertices) / quad_Layout.GetStride(),
			quad_Layout,
			quadIndexes, sizeof(quadIndexes) / sizeof(unsigned int)));


		FBO.reset(KEngine::FrameBuffer::Create());
		
		texture.reset(KEngine::Texture2D::Create());
		texture->AddToFrameBuffer(FBO);
		RBO.reset(KEngine::RenderBuffer::Create());

		std::vector<std::string> faces;
		faces.push_back("references\\skybox\\right.jpg");
		faces.push_back("references\\skybox\\left.jpg");
		faces.push_back("references\\skybox\\top.jpg");
		faces.push_back("references\\skybox\\bottom.jpg");
		faces.push_back("references\\skybox\\back.jpg");
		faces.push_back("references\\skybox\\front.jpg");

		textureCube.reset(KEngine::TextureCube::Create());
		textureCube->LoadCubemap(faces);
		

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
		sky_Mesh.reset(new KEngine::Mesh(sky_Vertices, sizeof(sky_Vertices) / sky_Layout.GetStride() ,
			sky_Layout,
			sky_Indices, sizeof(sky_Indices) / sizeof(unsigned int)));


		shaderList.clear();
		shaderList = {
			m_Shader,
			l_Shader,
			s_Shader,
			pointShader
		};
		//一个shader的列表,为他们每一个绑定相同的VP矩阵
		for (auto& shader : shaderList)
		{
			shader->BindUniformBufferPoint("VPMatrix",0);
		}
		//生成uniform缓冲对象
		matrixUBO.reset(KEngine::UniformBuffer::Create(2*sizeof(glm::mat4)));

	}
	void OnAttach() override {
		KEngine::Renderer::Init();
		lightPosition = glm::vec3(1.2f, 1.0f, 0.0f);
	}
	void OnUpdate(KEngine::TimeStep ts) {
		
		FBO->Bind();
		KEngine::Renderer::BeginScene();

		mainCamera->Control(ts.GetTimeStep());
		
		//填充数据到uniform缓冲对象
		//这个函数感觉得改成一个模板函数？
		matrixUBO->AddUniformData(CalculateVP(mainCamera->GetViewMatrix(), projMatrix), 0);
		
		//天空盒
		textureCube->Bind();
		sky_Shader->SetUniformMatrix4fv(glm::translate(glm::mat4(1.0f),glm::vec3(0.0f)),"model");
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
		m_Shader->SetUniformMatrix4fv(glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f)),glm::vec3(0.3f)),"model");
		m_Shader->SetUniform3f({ 1.0f,0.5f,0.31f }, "objectColor");
		m_Shader->SetUniform3f({ 1.0f,1.0f,1.0f },  "lightColor");
		m_Shader->SetUniform3f( lightPosition, "lightPos");
		m_Shader->SetUniform3f(mainCamera->GetPosition(), "viewPos");
		KEngine::Renderer::SetStencilFunc(GL_ALWAYS, 1, 0xFF);
		KEngine::Renderer::SetStencilMask(0xFF);
		KEngine::Renderer::Submit(m_Shader, m_Mesh);

		////边框
		s_Shader->SetUniformMatrix4fv(glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f)),glm::vec3(0.32f)),"model");
		KEngine::Renderer::SetStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		KEngine::Renderer::SetStencilMask(0x00);
		KEngine::Renderer::SetDepthOpenOrClose(false);
		KEngine::Renderer::Submit(s_Shader, m_Mesh);

		FBO->Unbind();
		KEngine::Renderer::SetDepthOpenOrClose(false);
		KEngine::Renderer::SetStencilOpenOrClose(false);
		
		//屏幕
		texture->Bind();
		KEngine::Renderer::Submit(screenShader, quad_Mesh);
		
		KEngine::Renderer::EndScene();
	}
	void OnEvent(KEngine::Event& event) override {
	}
	void ImGuiRender()override {
	}
	glm::mat4 CalculateVP( glm::mat4 view, glm::mat4 proj) {
		return proj * view;
	}
	private:
		std::shared_ptr<KEngine::Shader> m_Shader;
		std::shared_ptr<KEngine::Mesh> m_Mesh;

		std::shared_ptr<KEngine::Shader> l_Shader;
		std::shared_ptr<KEngine::Mesh> l_Mesh;

		std::shared_ptr<KEngine::Shader> s_Shader;

		std::shared_ptr<KEngine::Shader> screenShader;
		std::shared_ptr<KEngine::Mesh>quad_Mesh;

		std::shared_ptr<KEngine::TextureCube>textureCube;
		std::shared_ptr<KEngine::Shader>sky_Shader;
		std::shared_ptr<KEngine::Mesh>sky_Mesh;

		std::shared_ptr<KEngine::FrameBuffer>FBO;
		std::shared_ptr<KEngine::Texture2D>texture;
		std::shared_ptr<KEngine::RenderBuffer>RBO;

		std::shared_ptr<KEngine::Shader>pointShader;
	
		std::shared_ptr<KEngine::UniformBuffer> matrixUBO;

		std::vector<std::shared_ptr<KEngine::Shader>> shaderList;

		glm::vec3 lightPosition;

		glm::mat4 projMatrix;
		std::unique_ptr<KEngine::Camera>  mainCamera;

};


class Sandbox : public KEngine::Application {


public:
	Sandbox() {
		PushLayer(new ExampleLayer());
		
	}
	~Sandbox() {
	}


};



KEngine::Application* KEngine::CreateApplication() {
	return new Sandbox();
}