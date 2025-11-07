#include "kepch.h" 
#include "Buffer.h"
#include "Renderer.h"
#include "Platforms/OpenGL/OpenGLBuffer.h"
namespace KEngine{
    VertexBuffer* VertexBuffer::Create(const std::vector<float>& data){
        switch(Renderer::GetAPI()){
        case RendererAPI::API::None:
                //KE_CORE_ASSERT(false,"RendererAPI::None is currently not supported");
                return nullptr;
        case RendererAPI::API::OpenGL:
                return new OpenGLVertexBuffer(data);
        }
        //KE_CORE_ASSERT(false,"Unknown RendererAPI");
        return nullptr;
    }
    IndexBuffer* IndexBuffer::Create(std::vector<unsigned int> index){
        switch(Renderer::GetAPI()){
        case RendererAPI::API::None:
                //KE_CORE_ASSERT(false,"RendererAPI::None is currently not supported");
                return nullptr;
        case RendererAPI::API::OpenGL:
                return new OpenGLIndexBuffer(index);
        }
        //KE_CORE_ASSERT(false,"Unknown RendererAPI");
        return nullptr;
    }
    FrameBuffer* FrameBuffer::Create() {
        switch (Renderer::GetAPI()) {
        case RendererAPI::API::None:
            //KE_CORE_ASSERT(false, "RendererAPI::None is currently not supported");
            return nullptr;
        case RendererAPI::API::OpenGL:
            return new OpenGLFrameBuffer();
        }
        //KE_CORE_ASSERT(false, "Unknown RendererAPI");
        return nullptr;
    }
    RenderBuffer* RenderBuffer::Create(GLint type,const int width,const int height) {
        switch (Renderer::GetAPI()) {
        case RendererAPI::API::None:
            //KE_CORE_ASSERT(false, "RendererAPI::None is currently not supported");
            return nullptr;
        case RendererAPI::API::OpenGL:
            return new OpenGLRenderBuffer(type, width, height);
        }
        //KE_CORE_ASSERT(false, "Unknown RendererAPI");
        return nullptr;
    }
    UniformBuffer* UniformBuffer::Create(unsigned int size,unsigned int bindingPoint) {
        switch (Renderer::GetAPI()) {
        case RendererAPI::API::None:
            //KE_CORE_ASSERT(false, "RendererAPI::None is currently not supported");
            return nullptr;
        case RendererAPI::API::OpenGL:
            return new OpenGLUniformBuffer(size, bindingPoint);
        }
        //KE_CORE_ASSERT(false, "Unknown RendererAPI");
        return nullptr;
    }
}