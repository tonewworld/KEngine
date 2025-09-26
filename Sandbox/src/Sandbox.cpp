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

				void main()
				{
				   FragColor = ;
				}
			)";

			//m_Shader.reset(new KEngine::Shader(vertexSrc, fragmentSrc));

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
		

		m_VAO.reset(KEngine::VertexArray::Create());
		m_VAO->Bind();

		float vertices[] = {
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
		std::shared_ptr<KEngine::VertexBuffer> m_VBO;
		m_VBO.reset(KEngine::VertexBuffer::Create(vertices,sizeof(vertices)));
		KEngine::BufferLayout layout = { 
			{KEngine::ShaderDataType::Float3,"position"},
			{KEngine::ShaderDataType::Float3,"normal"}};
		m_VBO->SetLayout(layout);
		m_VAO->AddVertexBuffer(m_VBO);
		

		l_VAO.reset(KEngine::VertexArray::Create());
		l_VAO->Bind();
		std::shared_ptr<KEngine::VertexBuffer> l_VBO;
		l_VBO.reset(KEngine::VertexBuffer::Create(vertices, sizeof(vertices)));
		KEngine::BufferLayout l_Layout = {
			{KEngine::ShaderDataType::Float3,"position"} };
		l_VBO->SetLayout(l_Layout);
		l_VAO->AddVertexBuffer(l_VBO);

		
	}
	void OnAttach() override {
		KEngine::Renderer::Init();
		lightPosition = glm::vec3(1.2f, 1.0f, 2.0f);
	}
	void OnUpdate(KEngine::TimeStep ts) {
		
		KEngine::Renderer::BeginScene();
		
		mainCamera->Control(ts.GetTimeStep());

		l_Shader->SetUniformMatrix4fv(CalculateMVP(glm::scale(glm::translate(glm::mat4(1.0f),glm::vec3(1.2f,1.0f,2.0f)),glm::vec3(0.2f)), mainCamera->GetViewMatrix(), projMatrix), "MVP");//��������
		l_Shader->Bind();
		l_VAO->Bind();
		glDrawArrays(GL_TRIANGLES, 0, 36);
	
		//m_Shader->SetUniformMatrix4fv(CalculateMVP(glm::mat4(1.0f),mainCamera->GetViewMatrix(), projMatrix), "MVP");//��������
		//KEngine::Renderer::Submit(m_Shader, m_VAO);
		//	
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