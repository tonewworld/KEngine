#include "kepch.h"
#include "OpenGLRendererAPI.h"
#include "glad/glad.h"
namespace KEngine
{
	
	void OpenGLRendererAPI::Init()
	{

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

	void OpenGLRendererAPI::SetStencilOpenOrClose(bool tag) {
		if (tag)
		{
			glEnable(GL_STENCIL_TEST);
			return;
		}
		glDisable(GL_STENCIL_TEST);
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
			glDepthFunc(GL_LESS);
			return;
		}
			glDisable(GL_DEPTH_TEST);
	}
	void OpenGLRendererAPI::SwitchFrameBuffer(unsigned int frameBuffer) {
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // Set clear color to white (not really necessery actually, since we won't be able to see behind the quad anyways)
		glClear(GL_COLOR_BUFFER_BIT);
		

	}
}
