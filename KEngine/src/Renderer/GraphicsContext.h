#pragma once
#include "Core.h"

namespace KEngine {
	class KE_API GraphicsContext {
	public:
		GraphicsContext() = default;
		virtual ~GraphicsContext() = default;
		virtual void Init() = 0;
		virtual void SwapBuffers() = 0;
	};
}