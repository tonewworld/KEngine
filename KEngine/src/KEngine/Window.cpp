#include"kepch.h"
#include"Window.h"

#ifdef KENGINE_PLATFORM_WINDOWS
    #include "WindowsWindow.h"
#endif

namespace KEngine{
    inline std::unique_ptr<Window> KEngine::Window::Create(const WindowProps &props)
    {
	    #ifdef HZ_PLATFORM_WINDOWS
		return std::make_unique<WindowsWindow>(props);
	    //#else
		    //这里放一个assert
		return nullptr;
	    #endif
    }
}