#include"kepch.h"
#include"Window.h"


inline std::unique_ptr<Window> KEngine::Window::Create(const WindowProps &props)
{
    return std::unique_ptr<Window>();
}