#pragma once

#include "Core.h"
#include "Events/Event.h"
#include"Window.h"
#include"Layer.h"
#include "LayerStack.h"
#include "Events/ApplicationEvent.h"

namespace KEngine
{

	class KE_API Application
	{
	private:
		std::unique_ptr<Window> m_Window;
		LayerStack m_LayerStack;

		static Application* s_Instance;
	public:

		Application();
		virtual~Application();

		void Run();
		bool m_Running = true;

		void OnEvent(Event& e);

		bool OnWindowClose(WindowCloseEvent& e);

		void PushLayer(Layer* layer);
		void PushOverlayer(Layer* layer);

		inline Window& GetWindow() { return *m_Window; }
		inline static Application& GetApplication() { return *s_Instance; }

	};
	Application* CreateApplication();
	
}

