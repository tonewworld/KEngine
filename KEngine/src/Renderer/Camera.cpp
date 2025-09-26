#include "kepch.h"
#include "Camera.h"
#include "KeyCode.h"
#include "MouseButtonCode.h"
namespace KEngine
{
	
	void Camera::Control(float timeStep)
	{
		glm::vec3 temp = glm::vec3(GetPosition());
		float YawRotaion = GetYawRotation();
		float PitchRotation = GetPitchRotation();
		
		std::pair<float, float> mousePosition = KEngine::Input::GetMousePosition();
		float mouseX = mousePosition.first;
		float mouseY = mousePosition.second;
		
		// 如果鼠标右键按下，调整 yaw 和 pitch
		if (KEngine::Input::IsMouseButtonPressed(KE_MOUSE_BUTTON_RIGHT))
		{
			
			float deltaX = mouseX - lastMouseX;
			float deltaY = mouseY - lastMouseY;

			
			YawRotaion -= deltaX * mouseSensitivity;
			PitchRotation -= deltaY * mouseSensitivity;

			
			PitchRotation = std::clamp(PitchRotation, -89.0f, 89.0f);
			
		}

		lastMouseX = mouseX;
		lastMouseY = mouseY;

		if (KEngine::Input::IsKeyPressed(KE_KEY_A)) {
			temp -= GetRight() * moveSpeed * timeStep;
		}
		else if (KEngine::Input::IsKeyPressed(KE_KEY_D)) {
			temp += GetRight() * moveSpeed * timeStep;
		}
		if (KEngine::Input::IsKeyPressed(KE_KEY_W)) {
			temp += GetFront() * moveSpeed * timeStep;
		}
		else if (KEngine::Input::IsKeyPressed(KE_KEY_S)) {
			temp -= GetFront() * moveSpeed * timeStep;
		}

		SetPosition(temp);
		SetYawRotation(YawRotaion);
		SetPitchRotation(PitchRotation);
		CalculateViewMatrix();
	}
	void Camera::CalculateViewMatrix()
	{
		
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), glm::radians(yawRotation), glm::vec3(0, 1, 0))
			* glm::rotate(glm::mat4(1.0f), glm::radians(pitchRotation), glm::vec3(1, 0, 0));
	
		SetViewMatrix(glm::inverse(transform));
	}
}
