#include "kepch.h"
#include "OpenGLRendererAPI.h"
#include "glad/glad.h"

void KEngine::OpenGLRendererAPI::Init()
{
	
}

void KEngine::OpenGLRendererAPI::SetClearColor(float r, float g, float b, float a)
{
	glClearColor(r, g, b, a);
}

void KEngine::OpenGLRendererAPI::Clear()
{
	glClear(GL_COLOR_BUFFER_BIT);
}

void KEngine::OpenGLRendererAPI::DrawIndexed(const std::shared_ptr<class VertexArray>& vertexArray)
{
	glDrawElements(GL_TRIANGLES, vertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
}
