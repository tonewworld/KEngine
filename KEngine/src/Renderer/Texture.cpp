#include "kepch.h"
#include "Texture.h"
#include "Texture2D.h"
#include "TextureCube.h"

namespace KEngine {
	Texture::Texture() {
		glGenTextures(1, &m_RendererID);
	}
	
	Texture* Texture::Create(std::string& type) {

		if( type == "Texture2D")
		{
			
			return new Texture2D();
		}
		if (type == "TextureCube")
		{
			
			return new TextureCube();
		}

		return NULL;
	}
}