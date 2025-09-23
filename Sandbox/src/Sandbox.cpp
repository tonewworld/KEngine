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
				layout(location=0) in vec3 aPos;
				layout(location=1) in vec4 aColor;
				uniform mat4 ViewProjMatrix;
				out vec4 v_Color;
				void main()
				{
				   v_Color=aColor;
				   gl_Position = ViewProjMatrix * vec4(aPos.x,aPos.y,aPos.z,1.0);
				}
			)";
			char* fragmentSrc = R"(#version 330 core
				out vec4 FragColor;
				in vec4 v_Color;
				void main()
				{
				   FragColor = v_Color;
				}
			)";

			m_Shader.reset(new KEngine::Shader(vertexSrc, fragmentSrc));

		}
		{
			char* blueShaderVertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;
			uniform mat4 ViewProjMatrix;
			

			void main()
			{
				gl_Position= ViewProjMatrix * vec4(a_Position.x,a_Position.y,a_Position.z,1.0);	
			}
		)";

			char* blueShaderFragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;

			void main()
			{
				color = vec4(0.2, 0.3, 0.8, 1.0);
			}
		)";

			m_BlueShader.reset(new KEngine::Shader(blueShaderVertexSrc, blueShaderFragmentSrc));
		}

		m_VAO.reset(KEngine::VertexArray::Create());
		m_VAO->Bind();

		float vertices[3 * 7] = {
			-0.5f, -0.5f, -1.0f, 0.1f, 0.2f, 0.8f, 1.0f,
			 0.5f, -0.5f, -1.0f, 0.2f, 0.3f, 0.8f, 1.0f,
			 0.0f,  0.0f, -1.0f, 0.1f, 0.8f, 0.2f, 1.0f
		};
		std::shared_ptr<KEngine::VertexBuffer> m_VBO;
		m_VBO.reset(KEngine::VertexBuffer::Create(vertices,sizeof(vertices)));
		KEngine::BufferLayout layout = { 
			{KEngine::ShaderDataType::Float3,"position"},
			{KEngine::ShaderDataType::Float4,"color"}};
		m_VBO->SetLayout(layout);

		m_VAO->AddVertexBuffer(m_VBO);
		
		unsigned int indexes[3] = { 0,1,2 };
		std::shared_ptr<KEngine::IndexBuffer> m_IBO;
		m_IBO.reset(KEngine::IndexBuffer::Create(indexes,sizeof(indexes)));
		m_VAO->SetIndexBuffer(m_IBO);
		

		m_SquareVAO.reset(KEngine::VertexArray::Create());
		float squareVertices[3 * 4] = {
			-0.5f, -0.5f, -1.0f,
			 0.5f, -0.5f, -1.0f,
			 0.5f,  0.5f, -1.0f,
			-0.5f,  0.5f, -1.0f
		};

		std::shared_ptr<KEngine::VertexBuffer> squareVBO;
		squareVBO.reset(KEngine::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
		squareVBO->SetLayout({
			{ KEngine::ShaderDataType::Float3, "a_Position" }
			});
		m_SquareVAO->AddVertexBuffer(squareVBO);

		uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
		std::shared_ptr<KEngine::IndexBuffer> squareIBO;
		squareIBO.reset(KEngine::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
		m_SquareVAO->SetIndexBuffer(squareIBO);

	}
	void OnAttach() override {
		KEngine::Renderer::Init();
	}
	void OnUpdate(KEngine::TimeStep ts) {
		
		KEngine::Renderer::BeginScene();
		
		mainCamera->Control(ts.GetTimeStep());

		m_BlueShader->SetUniformMatrix4fv(CalculateMVP(glm::mat4(1.0f), mainCamera->GetViewMatrix(), projMatrix), "ViewProjMatrix");//��������
		KEngine::Renderer::Submit(m_BlueShader,m_SquareVAO);
			
		m_Shader->SetUniformMatrix4fv(CalculateMVP(glm::mat4(1.0f),mainCamera->GetViewMatrix(), projMatrix), "ViewProjMatrix");//��������
		KEngine::Renderer::Submit(m_Shader, m_VAO);
			
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

	std::shared_ptr<KEngine::Shader>m_BlueShader;
	std::shared_ptr<KEngine::VertexArray> m_SquareVAO;

	glm::mat4 projMatrix;
	std::unique_ptr<KEngine::Camera>  mainCamera;//δ�����������Դ���������

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