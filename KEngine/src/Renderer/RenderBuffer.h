#pragma once
#include "core.h"

namespace KEngine {

	class KE_API RenderBuffer
	{
	private:
		unsigned int m_RendererID;
	public:
		RenderBuffer();
		void Bind();
		void Unbind();
		static RenderBuffer* Create();
	};
}

