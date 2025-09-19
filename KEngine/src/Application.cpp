#include"kepch.h"
#include "Application.h"
#include "Log.h"
#include "glad/glad.h"
#include <GLFW/glfw3.h>


namespace KEngine {
	

	Application* Application::s_Instance = nullptr;

	Application::Application() {
		s_Instance = this;
		m_Window = std::unique_ptr<Window>(Window::Create(WindowProps()));
		m_Window->SetEventCallback(KE_BIND_FN(Application::OnEvent));

		m_ImGuiLayer=new ImGuiLayer();
		PushOverlayer(m_ImGuiLayer);
		
		{
			char* vertexSrc = "#version 330 core\n"
				"layout(location=0) in vec3 aPos;\n"
				"layout(location=1) in vec4 aColor;\n"
				"out vec4 v_Color;\n"
				"void main()\n"
				"{\n"
				"   v_Color=aColor;\n"
				"   gl_Position = vec4(aPos.x,aPos.y,aPos.z,1.0);\n"
				"}\0";
			char* fragmentSrc = "#version 330 core\n"
				"out vec4 FragColor;\n"
				"in vec4 v_Color;\n"
				"void main()\n"
				"{\n"
				"   FragColor = v_Color;\n"
				"}\0";

			m_Shader.reset(new Shader(vertexSrc, fragmentSrc));

		}
		{
			char* blueShaderVertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;

			out vec3 v_Position;

			void main()
			{
				v_Position = a_Position;
				gl_Position = vec4(a_Position, 1.0);	
			}
		)";

			char* blueShaderFragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;

			in vec3 v_Position;

			void main()
			{
				color = vec4(0.2, 0.3, 0.8, 1.0);
			}
		)";

			m_BlueShader.reset(new Shader(blueShaderVertexSrc, blueShaderFragmentSrc));
		}

		m_VAO.reset(VertexArray::Create());
		m_VAO->Bind();

		float vertices[3 * 7] = {
			-0.5f, -0.5f, 0.0f, 0.1f, 0.2f, 0.8f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
			 0.0f,  0.5f, 0.0f, 0.1f, 0.8f, 0.2f, 1.0f
		};
		std::shared_ptr<VertexBuffer> m_VBO;
		m_VBO.reset(VertexBuffer::Create(vertices,sizeof(vertices)));
		BufferLayout layout = { 
			{ShaderDataType::Float3,"position"},
			{ShaderDataType::Float4,"color"}};
		m_VBO->SetLayout(layout);

		/*uint32_t index = 0;
		const auto& layout = m_VBO->GetLayout();
		for (const auto& element : layout)
		{
			glEnableVertexAttribArray(index);
			glVertexAttribPointer(index,
				element.GetComponentCount(),
				GL_FLOAT,
				element.normalized ? GL_TRUE : GL_FALSE,
				layout.GetStride(),
				(const void*)element.offset);
			index++;
		}*/
		m_VAO->AddVertexBuffer(m_VBO);
		
		unsigned int indexes[3] = { 0,1,2 };
		std::shared_ptr<IndexBuffer> m_IBO;
		m_IBO.reset(IndexBuffer::Create(indexes,sizeof(indexes)));
		m_VAO->SetIndexBuffer(m_IBO);

		m_SquareVAO.reset(VertexArray::Create());
		float squareVertices[3 * 4] = {
			-0.75f, -0.75f, 0.0f,
			 0.75f, -0.75f, 0.0f,
			 0.75f,  0.75f, 0.0f,
			-0.75f,  0.75f, 0.0f
		};

		std::shared_ptr<VertexBuffer> squareVBO;
		squareVBO.reset(VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
		squareVBO->SetLayout({
			{ ShaderDataType::Float3, "a_Position" }
			});
		m_SquareVAO->AddVertexBuffer(squareVBO);

		uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
		std::shared_ptr<IndexBuffer> squareIBO;
		squareIBO.reset(IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
		m_SquareVAO->SetIndexBuffer(squareIBO);


		glBindVertexArray(0);

	}
	Application::~Application() {
		
		
	}
	void Application::Run() {

		while (m_Running) {
			
			glClearColor(0.1f, 0.1f, 0.1f, 1);
			glClear(GL_COLOR_BUFFER_BIT);

			m_BlueShader->Bind();
			m_SquareVAO->Bind();
			glDrawElements(GL_TRIANGLES, m_SquareVAO->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);

			m_Shader->Bind();
			m_VAO->Bind();
			glDrawElements(GL_TRIANGLES, m_VAO->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);

			for(Layer* layer:m_LayerStack)
				layer->OnUpdate();

			m_ImGuiLayer->ImGuiBegin();
			for(Layer* layer:m_LayerStack)
				layer->ImGuiRender();
			m_ImGuiLayer->ImGuiEnd();		

			m_Window->OnUpdate();
		}
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(std::bind(&Application::OnWindowClose, this, std::placeholders::_1));
		for(auto it=m_LayerStack.end();it!=m_LayerStack.begin();)
		{
			(*--it)->OnEvent(e);
			if (e.Handled)
				break;
		}
		/*KE_CORE_TRACE("{0}", e.ToString());*/

	}
	bool Application::OnWindowClose(WindowCloseEvent& e) {
		
		m_Running = false;
		return true;
	}

	void Application::PushLayer(Layer* layer) {
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}
	void Application::PushOverlayer(Layer* layer) {
		m_LayerStack.PushOverlayer(layer);
		layer->OnAttach();
	}
	
}