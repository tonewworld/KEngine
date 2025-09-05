#pragma once
#include "KEngine/Window.h"
#include <GLFW/glfw3.h>
namespace KEngine {

    class WindowsWindow :public Window {
    public:
        struct WindowData {
            uint32_t width;
            uint32_t height;
            bool Vsync;
            EventCallbackFn EventCallback;
        };


        WindowsWindow(const WindowProps& props);

        virtual~WindowsWindow();

        void OnUpdate()override;

        uint32_t GetWidth() const override { return myData.width; };
        uint32_t GetHeight()const override { return myData.height; };

        inline void SetEventCallback(const EventCallbackFn& callback)override { myData.EventCallback = callback; }
        void SetVSync(bool sign) override;
        bool IsVSync() override;

    private:
        WindowData myData;
        GLFWwindow* m_Window;

        virtual void Init(const WindowProps& props);
        virtual void Shutdown();

    };


};