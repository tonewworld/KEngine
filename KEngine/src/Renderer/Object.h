#pragma once
#include "Core.h"
#include <glm.hpp>
#include <string>

namespace KEngine {

	class KE_API Object {
	private:
		static unsigned int IDCounter;
		unsigned int ID = 0;
		std::string name = "Object";
		glm::mat4 model = glm::mat4(1.0f);
	public:
		Object() { ID = ++IDCounter; }
		Object(const std::string& name) : name(name) { ID = ++IDCounter; }
		virtual ~Object() = default;

		inline unsigned int GetID() const { return ID; }
		inline const std::string& GetName() const { return name; }
		inline void SetName(const std::string& name) { this->name = name; }

		inline glm::mat4 GetModelMatrix() const { return model; }
		inline void SetModelMatrix(const glm::mat4& m) { model = m; }
	};
}