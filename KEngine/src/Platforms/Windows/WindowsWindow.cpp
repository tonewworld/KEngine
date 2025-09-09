#pragma once
#include "kepch.h"
#include "WindowsWindow.h"
#include "Log.h"
#include"Events/ApplicationEvent.h"
#include"Events/KeyEvent.h"
#include"Events/MouseEvent.h"


namespace KEngine{

    static bool GLFWInitialized=false;

    static void GLFWErrorCallback(int error, const char* description) {
        KE_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
	}

    Window* Window::Create(const WindowProps& props) {
        return new WindowsWindow(props);//目前只有windows
    }
    
    WindowsWindow::WindowsWindow(const WindowProps&props){
        Init(props);
    }
    void WindowsWindow::Init(const WindowProps& props) {
        myData.width = props.Width;
        myData.height = props.Height;
        myData.Vsync = true;
        if (!GLFWInitialized) {
            glfwInit();
            //这里放一个assert
            GLFWInitialized = true;
        }
        m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, props.Title.c_str(), nullptr, nullptr);
        glfwMakeContextCurrent(m_Window);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);//GLAD加载OpenGL函数指针

        glfwSetWindowUserPointer(m_Window, &myData);//将data和window绑定，然后回调的时候使用
        SetVSync(true);

        glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int keycode){
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            KeyTypedEvent event(keycode);
            data.EventCallback(event);
        });


        glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.width = width;
			data.height = height;
			WindowResizeEvent event(width, height);
			data.EventCallback(event);
            });

        glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            WindowCloseEvent event;
			data.EventCallback(event);
            });

        glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            switch (action) {
            case GLFW_PRESS: {
                KeyPressedEvent event(key, false);
                data.EventCallback(event);
                break;
            }
            case GLFW_RELEASE: {
                KeyReleasedEvent event(key);
                data.EventCallback(event);
                break;
            }
            case GLFW_REPEAT: {
                KeyPressedEvent event(key, true);
                data.EventCallback(event);
                break;
            }
            }
            });

        glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

            switch (action)
            {
            case GLFW_PRESS:
            {
                MouseButtonPressedEvent event(button);
                data.EventCallback(event);
                break;
            }
            case GLFW_RELEASE:
            {
                MouseButtonReleasedEvent event(button);
                data.EventCallback(event);
                break;
            }
            }
			});
        glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset){
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

            MouseScrolledEvent event((float)xOffset, (float)yOffset);
            data.EventCallback(event);
            });

        glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos){
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

            MouseMovedEvent event((float)xPos, (float)yPos);
            data.EventCallback(event);
            });



    }
    void WindowsWindow::Shutdown(){

    }
    WindowsWindow::~WindowsWindow(){

    }
    void WindowsWindow::OnUpdate(){
        glfwPollEvents();
        glfwSwapBuffers(m_Window);
    }
    void WindowsWindow::SetVSync(bool sign){
        if (sign)
            glfwSwapInterval(1);
        else
            glfwSwapInterval(0);
    }
    bool WindowsWindow::IsVSync(){
        return myData.Vsync;
    }

}