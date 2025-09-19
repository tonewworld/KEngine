#pragma once
#include "Core.h"   
#include "RenderCommand.h"

#include "Shader.h"
namespace KEngine{
   
    class KE_API Renderer{
    public:
        static void Init();
        static void BeginScene();
		static void EndScene();
        static void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray);

        inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
    private:
       
    };
}