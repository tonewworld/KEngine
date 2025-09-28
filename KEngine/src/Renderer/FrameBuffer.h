#pragma once
#include "core.h"
#include "glad/glad.h"
namespace KEngine {

	class KE_API FrameBuffer
	{
	private:
		unsigned int m_RendererID;
	public:
		FrameBuffer();
		void Bind();
		void Unbind();
		static FrameBuffer* Create();
	};
}

