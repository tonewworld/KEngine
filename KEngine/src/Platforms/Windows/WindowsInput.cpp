#include "kepch.h"
#include "WindowsInput.h"
#include "GLFW/glfw3.h"
bool KEngine::WindowsInput::IsKeyPressedImpl(int keycode)
{
    auto window= static_cast<GLFWwindow*>(Application::GetApplication().GetWindow().GetNativeWindow());
    auto state=glfwGetKey(window,keycode);
    return state==GLFW_PRESS||state==GLFW_REPEAT;
}

bool KEngine::WindowsInput::IsMouseButtonPressedImpl(int button)
{
    auto window= static_cast<GLFWwindow*>(Application::GetApplication().GetWindow().GetNativeWindow());
    auto state=glfwGetMouseButton(window,button);
    return state==GLFW_PRESS;
}

std::pair<float, float> KEngine::WindowsInput::GetMousePositionImpl()
{
    auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
    return {xpos,ypos};
}

float KEngine::WindowsInput::GetMouseXImpl()
{
    auto[x,y] = GetMousePositionImpl();
    return x;
}

float KEngine::WindowsInput::GetMouseYImpl()
{
    auto[x,y] = GetMousePositionImpl();
    return y;
}
