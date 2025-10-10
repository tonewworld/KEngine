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
    RenderBuffer* RenderBuffer::Create() {
        switch (Renderer::GetAPI()) {
        case RendererAPI::API::None:
            //KE_CORE_ASSERT(false, "RendererAPI::None is currently not supported");
            return nullptr;
        case RendererAPI::API::OpenGL:
            return new OpenGLRenderBuffer();
        }
        //KE_CORE_ASSERT(false, "Unknown RendererAPI");
        return nullptr;
    }
    UniformBuffer* UniformBuffer::Create(unsigned int size) {
        switch (Renderer::GetAPI()) {
        case RendererAPI::API::None:
            //KE_CORE_ASSERT(false, "RendererAPI::None is currently not supported");
            return nullptr;
        case RendererAPI::API::OpenGL:
            return new OpenGLUniformBuffer(size);
        }
        //KE_CORE_ASSERT(false, "Unknown RendererAPI");
        return nullptr;
    }
}