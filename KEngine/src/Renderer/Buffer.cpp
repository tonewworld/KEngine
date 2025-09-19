#include "kepch.h" 
#include "Buffer.h"
#include "Renderer.h"
#include "Platforms/OpenGL/OpenGLBuffer.h"
namespace KEngine{
    VertexBuffer* VertexBuffer::Create(float* vertices, unsigned int size){
        switch(Renderer::GetAPI()){
        case RendererAPI::API::None:
                //KE_CORE_ASSERT(false,"RendererAPI::None is currently not supported");
                return nullptr;
        case RendererAPI::API::OpenGL:
                return new OpenGLVertexBuffer(vertices,size);
        }
        //KE_CORE_ASSERT(false,"Unknown RendererAPI");
        return nullptr;
    }
    IndexBuffer* IndexBuffer::Create(unsigned int* indexes, unsigned int size){
        switch(Renderer::GetAPI()){
        case RendererAPI::API::None:
                //KE_CORE_ASSERT(false,"RendererAPI::None is currently not supported");
                return nullptr;
        case RendererAPI::API::OpenGL:
                return new OpenGLIndexBuffer(indexes,size);
        }
        //KE_CORE_ASSERT(false,"Unknown RendererAPI");
        return nullptr;
    }
}