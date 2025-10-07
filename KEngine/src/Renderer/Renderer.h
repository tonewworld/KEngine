#pragma once
#include "Core.h"   
#include "RenderCommand.h"
#include "glm.hpp"
#include "Shader.h"

namespace KEngine{
   
    class KE_API Renderer{
    public:
        static void Init();
        static void BeginScene();
		static void EndScene();
        static void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray);
        
        static void SetStencilOpenOrClose(bool tag);
        static void SetStencilFunc(GLenum func,GLint ref,GLuint mask);
        static void SetStencilMask(GLint tag);

        static void SetDepthOpenOrClose(bool tag);
        
        static void SwitchFrameBuffer(unsigned int frameBuffer);

        static void Test();
        
        inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
    private:
       
    };
}