#pragma once
#include "Core.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include "Renderer/Shader.h"
#include "Renderer/Texture.h"
namespace KEngine {
	struct KE_API Material {
		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
		float shininess;
	};
	struct alignas(16) KE_API MaterialUboData {
		glm::vec3 ambient; float _pad0;
		glm::vec3 diffuse; float _pad1;
		glm::vec3 specular;float _pad2;
		float shininess;   float _pad3[3];
		MaterialUboData(Material& m)
			:ambient(m.ambient), _pad0(0),
			diffuse(m.diffuse), _pad1(0),
			specular(m.specular), _pad2(0),
			shininess(m.shininess), _pad3{ 0,0,0 } {
		}
	};

	class KE_API Object : public std::enable_shared_from_this<Object> {
	private:

		glm::vec3 m_Position = glm::vec3(0.0f);
		glm::vec3 m_Rotation = glm::vec3(0.0f); // 欧拉角，单位：度
		glm::vec3 m_Scale = glm::vec3(1.0f);

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
		bool isLight = false;
		std::shared_ptr<Texture> texture;
	protected:
		Material material;
	public:
		Object() { ID = ++IDCounter; }
		Object(const std::string& name) : name(name) { ID = ++IDCounter; }
		virtual ~Object() = default;

		void SetPosition(const glm::vec3& position) {
			m_Position = position;
		}
		void SetRotation(const glm::vec3& rotation) {
			m_Rotation = rotation;
		}
		void SetScale(const glm::vec3& scale) {
			m_Scale = scale;
		}

		glm::vec3 GetPosition() const { return m_Position; }
		glm::vec3 GetRotation() const { return m_Rotation; }
		glm::vec3 GetScale() const { return m_Scale; }

		inline unsigned int GetID() const { return ID; }
		inline const std::string& GetName() const { return name; }
		inline void SetName(const std::string& name) { this->name = name; }

		inline glm::mat4 GetModelMatrix() const { return model; }
		inline void SetModelMatrix(const glm::mat4& mat) { model = mat; }
		inline void UpdateModelMatrix() {
			model = glm::mat4(1.0f);
			model = glm::translate(model, m_Position);
			model = glm::rotate(model, glm::radians(m_Rotation.x), glm::vec3(1.0f, 0.0f, 0.0f)); // X轴
			model = glm::rotate(model, glm::radians(m_Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f)); // Y轴  
			model = glm::rotate(model, glm::radians(m_Rotation.z), glm::vec3(0.0f, 0.0f, 1.0f)); // Z轴
			model = glm::scale(model, m_Scale);
		}

		inline Material& GetMaterial() { return material; }
		inline void SetMaterial(const Material& mat) { material = mat; }

		std::shared_ptr<Shader> shader;
		void SetDrawState(std::shared_ptr<Texture>texture = nullptr,
			std::shared_ptr<Shader>shader = nullptr,
			bool depthTest = false, bool stencilTest = false,
			unsigned int stencilMask = 0,
			GLenum func = GL_ALWAYS, GLint ref = 0, GLuint mask = 0xFF);
		void Draw(std::shared_ptr<Shader>);

		bool GetIsLight() { return isLight; }
		void SetIsLight(bool tag) { isLight = tag; }
	};
}