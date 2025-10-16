#pragma once
#include "Core.h"   
#include "RenderCommand.h"
#include "glm.hpp"
#include "Shader.h"
#include "Model.h"
namespace KEngine{
   
    class KE_API Renderer{
    public:
        static void Init();
        static void BeginScene();
		static void EndScene();

        static void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<Mesh>& mesh);
        static void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<Model>& model);
        static void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<Object>& object);

       
        static void SetStencilOpenOrClose(bool tag);
        static void SetStencilFunc(GLenum func,GLint ref,GLuint mask);
        static void SetStencilMask(GLint tag);

        static void SetDepthOpenOrClose(bool tag);
        
        static void SwitchFrameBuffer(unsigned int frameBuffer);

        static std::array<unsigned char,4> ReadPixel(int rx,int ry);
		static void Test();
        
        inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
    private:
       
    };
}