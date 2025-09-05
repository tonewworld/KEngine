#pragma once

#include "Core.h"
#include "Events/Event.h"
#include"Window.h"

namespace KEngine
{

	class KE_API Application
	{
	private:
		std::unique_ptr<Window> m_Window;
	public:

		Application();
		virtual~Application();

		void Run();
		bool m_Running = true;

	};
	Application* CreateApplication();

}

