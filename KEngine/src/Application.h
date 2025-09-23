#pragma once

#include "Core.h"
#include "Events/Event.h"
#include "Window.h"
#include "Layer.h"
#include "ImGui/ImGuiLayer.h"
#include "LayerStack.h"
#include "Events/ApplicationEvent.h"
#include "Renderer/VertexArray.h"



namespace KEngine
{

	class KE_API Application
	{
	private:
		std::unique_ptr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		LayerStack m_LayerStack;

		
		float lastFrameTime=0.f;

	public:
		static Application* s_Instance;

		Application();
		virtual~Application();

		void Run();
		bool m_Running = true;

		void OnEvent(Event& e);

		bool OnWindowClose(WindowCloseEvent& e);

		void PushLayer(Layer* layer);
		void PushOverlayer(Layer* layer);

		inline Window& GetWindow() { return *m_Window; }
		static Application& GetApplication() { return *s_Instance; }

	protected:
		std::unique_ptr<TimeStep> timeStep;
		
	};
	Application* CreateApplication();
	
}

