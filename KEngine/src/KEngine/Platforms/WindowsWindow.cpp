#pragma once
#include "kepch.h"
#include "WindowsWindow.h"

namespace KEngine{

    static bool GLFWInitialized=false;

    Window* Window::Create(const WindowProps& props) {
        return new WindowsWindow(props);
    }
    
    WindowsWindow::WindowsWindow(const WindowProps&props){
        Init(props);
    }
    void WindowsWindow::Init(const WindowProps& props){
        myData.width=props.Width;
        myData.height=props.Height;
        myData.Vsync=true;
        if(!GLFWInitialized){
            glfwInit();
            //这里放一个assert
            GLFWInitialized=true;
        }
        m_Window=glfwCreateWindow((int)props.Width,(int)props.Height,props.Title.c_str(),nullptr,nullptr);
        glfwMakeContextCurrent(m_Window);
        glfwSetWindowUserPointer(m_Window,&myData);//将data和window绑定，然后回调的时候使用
        SetVSync(true);
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