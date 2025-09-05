#pragma once

#include "kepch.h"
#include "Core.h"
#include "Events/Event.h"

namespace KEngine{
    struct WindowProps{
        std::string Title;
        uint32_t Width; 
        uint32_t Height;
        WindowProps(const std::string& title="KEngine",uint32_t width=1600,uint32_t height=900)
            :Title(title),Width(width),Height(height){}

    };



    class  KE_API Window{
        
    public:
        
        using EventCallbackFn = std::function<void(Event&)>;
        Window()=default;
        virtual ~Window()=default;

        virtual void OnUpdate()=0;

        virtual uint32_t GetWidth() const=0; 
        virtual uint32_t GetHeight()const =0;

        virtual void SetVSync(bool sign)=0;
        virtual bool IsVSync()=0;

        virtual void SetEventCallback(const EventCallbackFn& callback)=0;

        
        static Window* Create(const WindowProps& props);

    };
}