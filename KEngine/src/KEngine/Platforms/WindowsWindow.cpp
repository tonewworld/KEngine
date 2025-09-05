#include "kepch.h"
#include "WindowsWindow.h"
#include <GLFW/glfw3.h>//也许能放进预编译头文件
namespace KEngine{

    static bool GLFWInitialized=false;
    
    WindowsWindow::WindowsWindow(const WindowProps&props){
        Init(props);
    }
    void WindowsWindow::Init(const WindowProps& props){
        myData.width=props.Width;
        myData.height=props.Height;
        myData.Vsync=true;
        if(!GLFWInitialized){
            int success=glfwInit();
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
        
    }
    void WindowsWindow::SetVSync(bool sign){

    }
    bool WindowsWindow::IsVSync(){
        return myData.Vsync;
    }

}