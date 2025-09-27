#include "kepch.h"
#include "OpenGLRendererAPI.h"
#include "glad/glad.h"
namespace KEngine
{
	
	void OpenGLRendererAPI::Init()
	{
		//…Ó∂»≤‚ ‘
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		//ƒ£∞Â≤‚ ‘
		glEnable(GL_STENCIL_TEST);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glStencilMask(0x00);
		
		//√ÊÃﬁ≥˝
		//glEnable(GL_CULL_FACE);
	}

	void OpenGLRendererAPI::SetClearColor(float r, float g, float b, float a)
	{
		glClearColor(r, g, b, a);
	}

	void OpenGLRendererAPI::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT| GL_STENCIL_BUFFER_BIT);
	}

	void OpenGLRendererAPI::DrawIndexed(const std::shared_ptr<class VertexArray>& vertexArray)
	{
		glDrawElements(GL_TRIANGLES, vertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLRendererAPI::SetStencilFunc(GLenum func, GLint ref, GLint mask) {
		glStencilFunc(func, ref, mask);
	}

	void OpenGLRendererAPI::SetStencilMask(GLint tag)
	{
		glStencilMask(tag);
	}

	void OpenGLRendererAPI::SetDepthOpenOrClose(bool tag)
	{	
		if (tag)
		{
			glEnable(GL_DEPTH_TEST);
			return;
		}
			glDisable(GL_DEPTH_TEST);
	}
}
