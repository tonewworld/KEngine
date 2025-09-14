#pragma once
#include "Renderer/GraphicsContext.h"
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include "Log.h"

namespace KEngine
{
	class KE_API OpenGLContext :public GraphicsContext
	{
	public:
		OpenGLContext(GLFWwindow* m_Handle);
		virtual ~OpenGLContext();
		virtual void Init() override;
		virtual void SwapBuffers() override;
	private:
		GLFWwindow* m_WindowHandle;
	};
}