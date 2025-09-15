#pragma once
#include "Core.h"   


namespace KEngine{
    enum class KE_API RendererAPI{
        None=0,
        OpenGL=1
        
    };
    class KE_API Renderer{
    public:
        inline static RendererAPI GetAPI(){return s_API;}
    private:
        static RendererAPI s_API;
    };
}