#pragma once 
#include "Core.h"
#include "Log.h"
#include "Renderer.h"

namespace KEngine{
    class KE_API VertexBuffer{
    public :
        VertexBuffer()=default;
        virtual ~VertexBuffer() = default;
        virtual void Bind() const =0;
        virtual void Unbind() const =0;
        static VertexBuffer* Create(float* vertices, unsigned int size);
    };
    class KE_API IndexBuffer{
    public :
        IndexBuffer()=default;
        virtual ~IndexBuffer() = default;
        virtual void Bind() const =0;
        virtual void Unbind() const =0;
        virtual unsigned int GetCount() const =0;
        static IndexBuffer* Create(unsigned int* indexes, unsigned int size);
    };
}