#include "kepch.h"
#include "Object.h"
#include "Renderer.h"
namespace KEngine {

    unsigned int Object::IDCounter = 0;
	void Object::SetDrawState(
		std::shared_ptr<Shader>shader,
		bool depthTest,
		bool stencilTest, unsigned int stencilMask, 
		GLenum func, GLint ref, GLuint mask)
	{
		this->shader = shader;
		this->depthTest = depthTest;
		this->stencilTest = stencilTest;
		this->stencilMask = stencilMask;
		this->func = func;
		this->ref = ref;
		this->mask = mask;
	}
	void Object::Draw(std::shared_ptr<Shader>shader)
    {
		Renderer::SetDepthOpenOrClose(depthTest);
		Renderer::SetStencilOpenOrClose(stencilTest);
		Renderer::SetStencilMask(stencilMask);
		Renderer::SetStencilFunc(func, ref, mask);
		shader->Bind();
		for (const auto& texture : textures) {
			texture->Bind(texture->GetTexSlot());
		}
		Renderer::Submit(shader, shared_from_this());
    }
}