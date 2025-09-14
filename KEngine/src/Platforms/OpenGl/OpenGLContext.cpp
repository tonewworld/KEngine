#include "kepch.h"
#include "OpenGLContext.h"
namespace KEngine
{
	KEngine::OpenGLContext::OpenGLContext(GLFWwindow*m_Handle):m_WindowHandle(m_Handle)
	{
	}

	KEngine::OpenGLContext::~OpenGLContext()
	{
	}

	void KEngine::OpenGLContext::Init()
	{
		glfwMakeContextCurrent(m_WindowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		KE_CORE_INFO(status);
	}

	void KEngine::OpenGLContext::SwapBuffers()
	{
		glfwSwapBuffers(m_WindowHandle);
	}

}
