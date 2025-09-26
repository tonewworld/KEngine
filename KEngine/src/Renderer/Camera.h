#pragma once
#include "Core.h"
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "Input.h"

namespace KEngine
{
	class KE_API Camera
	{
	public:
		Camera() :viewMatrix(glm::mat4(1.0f)) {}
		virtual ~Camera() = default;
		void Control(float timeStep);
		inline glm::vec3 GetPosition() const { return position; }
		inline void SetPosition(const glm::vec3& pos) { position = pos; }
		inline float GetRotation() const { return rotation; }
		inline void SetRotation(float rot) { rotation = rot; }

		inline glm::mat4 GetViewMatrix() const { return viewMatrix; }
		inline void SetViewMatrix(const glm::mat4& matrix) { viewMatrix = matrix; }

		void CalculateViewMatrix();
		
	private:
		glm::vec3 position = { 0,0,3 };
		float rotation = 0.f;

		float moveSpeed=0.4f;
		glm::mat4 viewMatrix;
	};

}

