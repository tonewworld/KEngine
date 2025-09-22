#pragma once
#include "Core.h"
#include "glm.hpp"
#include "VertexArray.h"
namespace KEngine
{

    class KE_API RendererAPI
    {
    public:
        enum class API {
            None = 0,
            OpenGL = 1

        };

		virtual ~RendererAPI() = default;

		virtual void Init() = 0;
		virtual void SetClearColor(float r, float g, float b, float a) = 0;
		virtual void Clear() = 0;
        virtual void DrawIndexed(const std::shared_ptr<class VertexArray>& vertexArray) = 0;
		
	    inline static API GetAPI() { return s_API ; }

    private:
		static API s_API;

    };

}

