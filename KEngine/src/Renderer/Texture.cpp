#include "kepch.h"
#include "Texture.h"
#include "Texture2D.h"
#include "TextureCube.h"
namespace KEngine {
	Texture::Texture() {
		glGenTextures(1, &m_RendererID);
	}
	
	Texture* Texture::Create(char* type) {

		if(strcmp(type, "Texture2D") == 0)
			return new Texture2D();
		if (strcmp(type, "TextureCube") == 0)
			return new TextureCube();

		return NULL;
	}
}