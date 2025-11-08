#pragma once
#include "Core.h"
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "Input.h"
#include "Application.h"
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

		inline float GetYawRotation() const { return yawRotation; }
		inline void SetYawRotation(float rot) { yawRotation = rot; }
		inline float GetPitchRotation() const { return pitchRotation; }
		inline void SetPitchRotation(float rot) { pitchRotation = rot; }

		inline glm::mat4 GetViewMatrix() const { return viewMatrix; }
		inline void SetViewMatrix(const glm::mat4& matrix) { viewMatrix = matrix; }

		inline glm::mat4 GetProjMatrix() const { return projMatrix; }
		inline void SetProjMatrix(const glm::mat4& matrix) { projMatrix = matrix; }

		static bool CheckLeftMouseButtonPress();

		void CalculateViewMatrix();

		inline glm::vec3 GetFront()const {
			glm::mat4 rotation=glm::rotate(glm::rotate(glm::mat4(1.0f), glm::radians(yawRotation), glm::vec3(0, 1, 0))
				,glm::radians(pitchRotation),glm::vec3(1,0,0));
			return glm::vec3(rotation * glm::vec4(0, 0, -1, 1));
		}
		inline glm::vec3 GetRight()const {
			glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(yawRotation), glm::vec3(0, 1, 0));

			return glm::vec3(rotation * glm::vec4(1, 0, 0, 1));
		}
		
	private:
		glm::vec3 position = { 0,1,3 };

		float yawRotation = 0.f;
		float pitchRotation = -30.f;

		float lastMouseX = 0.0f;
		float lastMouseY = 0.0f;
		float mouseSensitivity = 0.1f;

		float moveSpeed=0.4f;
		glm::mat4 viewMatrix;
		glm::mat4 projMatrix= glm::perspective(glm::radians(45.f), (float)
			KEngine::Application::s_Instance->GetWindow().GetWidth()
			/ KEngine::Application::s_Instance->GetWindow().GetHeight(),
			0.1f, 300.f);
	};

}

