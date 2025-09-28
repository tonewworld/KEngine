#include"kepch.h"
#include "Application.h"
#include "Log.h"
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include "Renderer/Renderer.h"

#include "glm.hpp"
#include "gtc/matrix_transform.hpp"

namespace KEngine {
	

	Application* Application::s_Instance = nullptr;

	Application::Application() {
		s_Instance = this;

		timeStep = std::make_unique<TimeStep>(0.f);

		m_Window = std::unique_ptr<Window>(Window::Create(WindowProps()));
		m_Window->SetEventCallback(KE_BIND_FN(Application::OnEvent));

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlayer(m_ImGuiLayer);

		
		glGenFramebuffers(1, &frameBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

		unsigned int textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1600.f, 900.f, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0);

		GLuint rbo;
		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1600.f, 900.f); // Use a single renderbuffer object for both a depth AND stencil buffer.
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // Now actually attach it
		// Now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	}
	Application::~Application() {
		
	}	

	void Application::Run() {
		std::shared_ptr<Shader>screenShader;
		{
			char* vertexSrc = R"(
				#version 330 core
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
				#version 330 core
				in vec2 TexCoords;
				out vec4 color;

				uniform sampler2D screenTexture;

				void main()
				{
					color =vec4(vec3(1.0 - texture(screenTexture, TexCoords)),1.0);
				}
				)";
			
			screenShader.reset(new Shader(vertexSrc, fragmentSrc));
		}

		unsigned int frameBuffer;
		glGenFramebuffers(1, &frameBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

		unsigned int textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1600.f, 900.f, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0);

		GLuint rbo;
		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1600.f, 900.f); // Use a single renderbuffer object for both a depth AND stencil buffer.
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // Now actually attach it
		// Now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		GLfloat quadVertices[] = {   // Vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
			// Positions   // TexCoords
			-1.0f,  1.0f,  0.0f, 1.0f,
			-1.0f, -1.0f,  0.0f, 0.0f,
			 1.0f, -1.0f,  1.0f, 0.0f,

			-1.0f,  1.0f,  0.0f, 1.0f,
			 1.0f, -1.0f,  1.0f, 0.0f,
			 1.0f,  1.0f,  1.0f, 1.0f
		};
		GLuint quadVAO, quadVBO;
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
		glBindVertexArray(0);

		glDepthFunc(GL_LESS);

		while (m_Running) {
			

			glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
			// Clear all attached buffers        
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glEnable(GL_DEPTH_TEST);

			float nowFrameTime=(float)glfwGetTime();
			timeStep->SetTimeStep(nowFrameTime-lastFrameTime);
			lastFrameTime=nowFrameTime;

			for(Layer* layer:m_LayerStack)
				layer->OnUpdate(*timeStep);

			m_ImGuiLayer->ImGuiBegin();
			for(Layer* layer:m_LayerStack)
				layer->ImGuiRender();
			m_ImGuiLayer->ImGuiEnd();		

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // Set clear color to white (not really necessery actually, since we won't be able to see behind the quad anyways)
			glClear(GL_COLOR_BUFFER_BIT);
			glDisable(GL_DEPTH_TEST); // We don't care about depth information when rendering a single quad

			screenShader->Bind();
			glBindVertexArray(quadVAO);
			glBindTexture(GL_TEXTURE_2D, textureID);	// Use the color attachment texture as the texture of the quad plane
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);

			m_Window->OnUpdate();
		}
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(KE_BIND_FN(Application::OnWindowClose));
		dispatcher.Dispatch<MouseButtonPressedEvent>(KE_BIND_FN(Application::OnRightMouseButtonPressed));
		dispatcher.Dispatch<MouseButtonReleasedEvent>(KE_BIND_FN(Application::OnRightMouseButtonReleased));
		dispatcher.Dispatch<WindowResizeEvent>(KE_BIND_FN(Application::OnWindowResize));
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
	bool Application::OnRightMouseButtonPressed(MouseButtonPressedEvent& e){
		if (e.GetMouseButton() == KE_MOUSE_BUTTON_RIGHT) {
			m_Window->SetCursorVisable(false);
			return false;
		}
		return false;
	}

	bool Application::OnRightMouseButtonReleased(MouseButtonReleasedEvent& e)
	{
		if (e.GetMouseButton() == KE_MOUSE_BUTTON_RIGHT) {
			m_Window->SetCursorVisable(true);
			return false;
		}
		return false;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		glViewport(0, 0, e.GetWidth(), e.GetHeight());
		return false;
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