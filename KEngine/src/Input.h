#pragma once

#include "Core.h"
#include <utility>

namespace KEngine {
    //input 是一个接口，不同平台都能用
    class KE_API Input {
    public:
        inline static bool IsKeyPressed(int keycode){return s_Instance->IsKeyPressedImpl(keycode);};
        inline static bool IsMouseButtonPressed(int button){return s_Instance->IsMouseButtonPressedImpl(button);};
        static std::pair<float, float> GetMousePosition(){return s_Instance->GetMousePositionImpl();};
        static float GetMouseX(){return s_Instance->GetMouseXImpl();};
        static float GetMouseY(){return s_Instance->GetMouseYImpl();};
    protected:
        virtual bool IsKeyPressedImpl(int keycode) = 0;
        virtual bool IsMouseButtonPressedImpl(int button) = 0;
        virtual std::pair<float, float> GetMousePositionImpl() = 0;
        virtual float GetMouseXImpl() = 0;
        virtual float GetMouseYImpl() = 0;

    private:
        static Input* s_Instance;


    };

}