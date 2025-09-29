#pragma once
#include"Texture.h"

namespace KEngine {

	class KE_API Texture2D:public Texture
	{
	public:
		Texture2D();
		~Texture2D();
		virtual void Bind() override ;
		virtual void Unbind( )override ;
		void AddToFrameBuffer(std::shared_ptr<FrameBuffer>fbo);
	};
}

