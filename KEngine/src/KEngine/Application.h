#pragma once

#include "Core.h"

namespace KEngine
{

	class KE_API Application
	{
	private:

	public:

		Application();
		virtual~Application();

		void Run();
		

	};
	Application* CreateApplication();

}

