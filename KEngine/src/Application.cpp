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
		
		char* vertexSrc = "#version 330 core\n"
			"layout(location=0) in vec3 aPos;\n"
			"void main()\n"
			"{\n"
			"   gl_Position = vec4(aPos.x,aPos.y,aPos.z,1.0);\n"
			"}\0";	
		char* fragmentSrc = "#version 330 core\n"	
			"out vec4 FragColor;\n"
			"void main()\n"
			"{\n"
			"   FragColor = vec4(1.0f,0.5f,0.2f,1.0f);\n"
			"}\0";

		m_Shader.reset(new Shader(vertexSrc, fragmentSrc));
		m_Shader->Bind();
		float vertices[3 * 3] = {
			-0.5f, -0.5f, 0.0f,
			 0.5f, -0.5f, 0.0f,
			 0.0f,  0.5f, 0.0f
		};

		unsigned int indices[3] = { 0,1,2 };

		
		glGenVertexArrays(1, &m_VAO);
		glBindVertexArray(m_VAO);

		glGenBuffers(1, &m_VBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (const void*)0);

		glGenBuffers(1, &m_IBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
		
		glBindVertexArray(0);

	}
	Application::~Application() {
		
		
	}
	void Application::Run() {

		while (m_Running) {
			
			glClearColor(0.1f, 0.1f, 0.1f, 1);
			glClear(GL_COLOR_BUFFER_BIT);


			glBindVertexArray(m_VAO);
			glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);

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