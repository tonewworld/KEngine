#include "kepch.h"
#include "VertexArray.h"

#include "Renderer.h"
#include "Platforms/OpenGL/OpenGLVertexArray.h"

namespace KEngine {

	VertexArray* VertexArray::Create()
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::None:    return nullptr;
		case RendererAPI::OpenGL:  return new OpenGLVertexArray();
		}

		return nullptr;
	}

}