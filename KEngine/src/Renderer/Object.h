#pragma once
#include "Core.h"
#include <glm.hpp>
#include "Renderer/Shader.h"
#include "Renderer/Texture.h"
namespace KEngine {

	class KE_API Object : public std::enable_shared_from_this<Object> {
	private:
		static unsigned int IDCounter;
		unsigned int ID = 0;
		std::string name = "Object";
		glm::mat4 model = glm::mat4(1.0f);

		bool depthTest = false;
		bool stencilTest = false;
		unsigned int stencilMask = 0;
		GLenum func = GL_ALWAYS;
		GLint ref = 0;
		GLuint mask = 0xFF;

		std::shared_ptr<Texture> texture;
	public:
		Object() { ID = ++IDCounter; }
		Object(const std::string& name) : name(name) { ID = ++IDCounter; }
		virtual ~Object() = default;

		inline unsigned int GetID() const { return ID; }
		inline const std::string& GetName() const { return name; }
		inline void SetName(const std::string& name) { this->name = name; }

		inline glm::mat4 GetModelMatrix() const { return model; }
		inline void SetModelMatrix(const glm::mat4& m) { model = m; }

		std::shared_ptr<Shader> shader;
		void SetDrawState(std::shared_ptr<Texture>texture = nullptr,
			std::shared_ptr<Shader>shader = nullptr,
			bool depthTest = false, bool stencilTest = false,
			unsigned int stencilMask = 0,
			GLenum func = GL_ALWAYS, GLint ref = 0, GLuint mask = 0xFF);
		void Draw(std::shared_ptr<Shader>);
	};
}