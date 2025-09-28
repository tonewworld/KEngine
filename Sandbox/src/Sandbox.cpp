#include<KEngine.h>
#include "imgui.h"
#include "glad/glad.h"
class ExampleLayer : public KEngine::Layer {
	public:
	ExampleLayer():Layer("Example") {
		mainCamera = std::make_unique<KEngine::Camera>();
		projMatrix = glm::perspective(glm::radians(45.f), 16.f / 9.f, 0.1f, 300.f);
		{
			char* vertexSrc = R"(
				#version 330 core
				layout(location=0) in vec3 v_Position;
				layout(location=1) in vec3 v_Normal;

				out vec3 Normal;
				out vec3 FragPos;
				
				uniform mat4 model;
				uniform mat4 MVP;
				
				void main()
				{
					gl_Position = MVP * vec4(v_Position,1.0);
					FragPos = v_Position;
					Normal=mat3(transpose(inverse(model))) * v_Normal;
				}
			)";
			char* fragmentSrc = R"(#version 330 core
				out vec4 FragColor;
				in vec3 FragPos;
				in vec3 Normal;

				uniform vec3 lightPos;
				uniform vec3 viewPos;
				uniform vec3 lightColor;
				uniform vec3 objectColor;
				

				void main()
				{
					// Ambient
					float ambientStrength = 0.1f;
					vec3 ambient = ambientStrength * lightColor;
  	
					// Diffuse 
					vec3 norm = normalize(Normal);
					vec3 lightDir = normalize(lightPos - FragPos);
					float diff = max(dot(norm, lightDir), 0.0);
					vec3 diffuse = diff * lightColor;
    
					// Specular
					float specularStrength = 0.5f;
					vec3 viewDir = normalize(viewPos - FragPos);
					vec3 reflectDir = reflect(-lightDir, norm);  
					float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
					vec3 specular = specularStrength * spec * lightColor;  
        
					vec3 result = (ambient+diffuse+specular) * objectColor;
					FragColor = vec4(result, 1.0f);
				}
			)";

			m_Shader.reset(new KEngine::Shader(vertexSrc, fragmentSrc));

		}
		{
			char* vertexSrc = R"(
				#version 330 core
				layout(location=0) in vec3 v_Position;
				
				uniform mat4 MVP;
				
				void main()
				{
					gl_Position = MVP * vec4(v_Position,1.0);
				}
			)";
			char* fragmentSrc = R"(
				#version 330 core
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
				#version 330 core
				layout(location=0) in vec3 v_Position;
				layout(location=1) in vec3 v_Normal;

				uniform mat4 model;
				uniform mat4 MVP;
				
				void main()
				{
					gl_Position = MVP * vec4(v_Position,1.0);
				}
			)";
			char* fragmentSrc = R"(#version 330 core
				out vec4 FragColor;
				
				void main()
				{
					FragColor = vec4(0.7f);
				}
			)";

			s_Shader.reset(new KEngine::Shader(vertexSrc, fragmentSrc));

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
		m_VAO.reset(KEngine::VertexArray::Create());
		m_VAO->Bind();

		std::shared_ptr<KEngine::VertexBuffer> m_VBO;
		m_VBO.reset(KEngine::VertexBuffer::Create(m_Vertices,sizeof(m_Vertices)));
		KEngine::BufferLayout layout = { 
			{KEngine::ShaderDataType::Float3,"position"},
			{KEngine::ShaderDataType::Float3,"normal"}};
		m_VBO->SetLayout(layout);
		m_VAO->AddVertexBuffer(m_VBO);

		unsigned int m_Indexes[]{
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
		std::shared_ptr<KEngine::IndexBuffer> m_IBO;
		m_IBO.reset(KEngine::IndexBuffer::Create(m_Indexes, sizeof(m_Indexes) / sizeof(unsigned int)));
		m_VAO->SetIndexBuffer(m_IBO);
		

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

		l_VAO.reset(KEngine::VertexArray::Create());
		l_VAO->Bind();

		std::shared_ptr<KEngine::VertexBuffer> l_VBO;
		l_VBO.reset(KEngine::VertexBuffer::Create(l_Vertices, sizeof(l_Vertices)));
		KEngine::BufferLayout l_Layout = {
			{KEngine::ShaderDataType::Float3,"position"} };
		l_VBO->SetLayout(l_Layout);
		l_VAO->AddVertexBuffer(l_VBO);

		unsigned int l_Indexes[]{
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
		std::shared_ptr<KEngine::IndexBuffer> l_IBO;
		l_IBO.reset(KEngine::IndexBuffer::Create(l_Indexes, sizeof(l_Indexes) / sizeof(unsigned int)));
		l_VAO->SetIndexBuffer(l_IBO);


	}
	void OnAttach() override {
		KEngine::Renderer::Init();
		lightPosition = glm::vec3(1.2f, 1.0f, 0.0f);
	}
	void OnUpdate(KEngine::TimeStep ts) {
		
		KEngine::Renderer::BeginScene();
		
		mainCamera->Control(ts.GetTimeStep());
		
		
		l_Shader->SetUniformMatrix4fv(CalculateMVP(glm::scale(glm::translate(glm::mat4(1.0f),lightPosition),glm::vec3(0.01f)), 
			mainCamera->GetViewMatrix(), 
			projMatrix), "MVP");
		//KEngine::Renderer::SetStencilMask(0);
		KEngine::Renderer::Submit(l_Shader, l_VAO);
	
		
		m_Shader->SetUniformMatrix4fv(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f)), "model");
		m_Shader->SetUniform3f({ 1.0f,0.5f,0.31f }, "objectColor");
		m_Shader->SetUniform3f({ 1.0f,1.0f,1.0f },  "lightColor");
		m_Shader->SetUniform3f( lightPosition, "lightPos");
		m_Shader->SetUniform3f(mainCamera->GetPosition(), "viewPos");
		m_Shader->SetUniformMatrix4fv(CalculateMVP(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f)),
			mainCamera->GetViewMatrix(), 
			projMatrix), "MVP");
		//KEngine::Renderer::SetStencilFunc(GL_ALWAYS, 1, 0xFF);
		//KEngine::Renderer::SetStencilMask(0xFF);
		KEngine::Renderer::Submit(m_Shader, m_VAO);

	/*	s_Shader->SetUniformMatrix4fv(CalculateMVP(glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f)),glm::vec3(1.1f)),
			mainCamera->GetViewMatrix(),
			projMatrix), "MVP");
		KEngine::Renderer::SetStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		KEngine::Renderer::SetStencilMask(0x00);
		KEngine::Renderer::SetDepthOpenOrClose(false);
		KEngine::Renderer::Submit(s_Shader, m_VAO);*/

		

		KEngine::Renderer::EndScene();
	}
	void OnEvent(KEngine::Event& event) override {
	}
	void ImGuiRender()override {
	}
	glm::mat4 CalculateMVP(glm::mat4 model, glm::mat4 view, glm::mat4 proj) {
		return proj * view * model;
	}
	private:
	std::shared_ptr<KEngine::Shader> m_Shader;
	std::shared_ptr<KEngine::VertexArray> m_VAO;

	std::shared_ptr<KEngine::Shader> l_Shader;
	std::shared_ptr<KEngine::VertexArray>l_VAO;

	std::shared_ptr<KEngine::Shader> s_Shader;
	
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