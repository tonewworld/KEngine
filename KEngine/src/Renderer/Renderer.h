#pragma once
#include "Core.h"   
#include "RenderCommand.h"
#include "glm.hpp"
#include "Shader.h"
#include "Model.h"
#include "Light.h"
namespace KEngine{
   
    class KE_API Renderer{
    public:
        static void Init();
        static void BeginScene();
		static void EndScene();

		static void ColorPickBegin();
        static void ParallelLightShadowBegin();
        static void ParallelLightShadowEnd();
        static void PointLightShadowBegin();
        static void PointLightShadowEnd();

        //重构
        static void GeometryPassBegin();
        static void GeometryPassEnd();
        static void SSAOPassBegin();
        static void SSAOPassEnd();
        static void SSAOBlurPassBegin();
        static void SSAOBlurPassEnd();
        static void LightingPassBegin();
        static void LightingPassEnd();
        static void HDRandBloomBegin();
        static void HDRandBloomEnd();
        static void ScreenPassBegin();
        static void ScreenPassEnd();

        //前向渲染
        static void ForwardRenderPassBegin();
        static void ForwardRenderPassEnd();
        static void BlitFrameBuffer(std::shared_ptr<FrameBuffer>,unsigned int width,unsigned int height);

        static void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<Mesh>& mesh);
        static void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<Model>& model);
        static void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<Light>& model);
        static void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<Object>& object);
       
        static void SetStencilOpenOrClose(bool tag);
        static void SetStencilFunc(GLenum func,GLint ref,GLuint mask);
        static void SetStencilMask(GLint tag);

        static void SetDepthOpenOrClose(bool tag);
        
        static void SwitchFrameBuffer(unsigned int frameBuffer);

        static std::array<unsigned char,4> ReadPixel(int rx,int ry);
        
        static void ResetGLState();
        inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
        static void Debug();
    private:
       
    };
}