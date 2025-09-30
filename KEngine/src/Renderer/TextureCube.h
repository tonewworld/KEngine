#pragma once
#include "Texture.h"
namespace KEngine {

	class KE_API TextureCube:public Texture
	{
	public:
		TextureCube();
		~TextureCube();
		virtual void Bind() override;
		virtual void Unbind() override;
		void LoadCubemap(std::vector<std::string>&faces);
	};
}

