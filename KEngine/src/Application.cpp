#include"kepch.h"
#include "Application.h"
#include "Log.h"
#include "glad/glad.h"
#include <GLFW/glfw3.h>


namespace KEngine {

	Application* Application::s_Instance = nullptr;

	static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
	{
		switch (type)
		{
			case KEngine::ShaderDataType::Float:    return GL_FLOAT;
			case KEngine::ShaderDataType::Float2:   return GL_FLOAT;
			case KEngine::ShaderDataType::Float3:   return GL_FLOAT;
			case KEngine::ShaderDataType::Float4:   return GL_FLOAT;
			case KEngine::ShaderDataType::Mat3:     return GL_FLOAT;
			case KEngine::ShaderDataType::Mat4:     return GL_FLOAT;
			case KEngine::ShaderDataType::Int:      return GL_INT;
			case KEngine::ShaderDataType::Int2:     return GL_INT;
			case KEngine::ShaderDataType::Int3:     return GL_INT;
			case KEngine::ShaderDataType::Int4:     return GL_INT;
			case KEngine::ShaderDataType::Bool:     return GL_BOOL;
		}
		
		return 0;
	}

	Application::Application() {
		s_Instance = this;
		m_Window = std::unique_ptr<Window>(Window::Create(WindowProps()));
		m_Window->SetEventCallback(KE_BIND_FN(Application::OnEvent));

		m_ImGuiLayer=new ImGuiLayer();
		PushOverlayer(m_ImGuiLayer);
		
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
		m_Shader->Bind();
		float vertices[3 * 7] = {
			-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
			 0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
		};

		unsigned int indexes[3] = { 0,1,2 };

		
		glGenVertexArrays(1, &m_VAO);
		glBindVertexArray(m_VAO);

		m_VBO.reset(VertexBuffer::Create(vertices,sizeof(vertices)));
		BufferLayout layout = { 
			{ShaderDataType::Float3,"position"},
			{ShaderDataType::Float4,"color"}};
		m_VBO->SetLayout(layout);

		unsigned int index=0;
		const auto& elements = m_VBO->GetLayout().GetElements();
		for (const auto& element : elements){
			glEnableVertexAttribArray(index);
			glVertexAttribPointer(index,
				element.GetComponentCount(),
				ShaderDataTypeToOpenGLBaseType(element.type),
				element.normalized,layout.GetStride(),
				(const void*)element.offset);
			index++;
		}
		
		m_IBO.reset(IndexBuffer::Create(indexes,sizeof(indexes)));
		
		glBindVertexArray(0);

	}
	Application::~Application() {
		
		
	}
	void Application::Run() {

		while (m_Running) {
			
			glClearColor(0.1f, 0.1f, 0.1f, 1);
			glClear(GL_COLOR_BUFFER_BIT);


			glBindVertexArray(m_VAO);
			glDrawElements(GL_TRIANGLES, m_IBO->GetCount(), GL_UNSIGNED_INT, nullptr);

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