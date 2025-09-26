#include "kepch.h"
#include "Camera.h"
#include "KeyCode.h"
#include "MouseButtonCode.h"
namespace KEngine
{
	
	void Camera::Control(float timeStep)
	{
		glm::vec3 temp = glm::vec3(GetPosition());
		float tempRotaion = GetRotation();
		if(KEngine::Input::IsKeyPressed(KE_KEY_LEFT)){
			temp.x -= moveSpeed * timeStep;
		}else if(KEngine::Input::IsKeyPressed(KE_KEY_RIGHT)){
			temp.x += moveSpeed * timeStep;
		}
		if(KEngine::Input::IsKeyPressed(KE_KEY_UP)){
			temp.y += moveSpeed * timeStep;
		}else if(KEngine::Input::IsKeyPressed(KE_KEY_DOWN)){
			temp.y -= moveSpeed * timeStep;
		}
		if (KEngine::Input::IsMouseButtonPressed(KE_MOUSE_BUTTON_LEFT)) {
			temp.z += moveSpeed * timeStep;
		}
		else if (KEngine::Input::IsMouseButtonPressed(KE_MOUSE_BUTTON_RIGHT)) {
			temp.z -= moveSpeed * timeStep;
		}
		if (KEngine::Input::IsKeyPressed(KE_KEY_A)) {
			tempRotaion += 40.0f * moveSpeed * timeStep;
		}
		else if (KEngine::Input::IsKeyPressed(KE_KEY_D)) {
			tempRotaion -= 40.0f * moveSpeed * timeStep;
		}

		SetPosition(temp);
		SetRotation(tempRotaion);
		CalculateViewMatrix();
	}
	void Camera::CalculateViewMatrix()
	{
		
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) 
			* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), glm::vec3(0, 1, 0));
	
		SetViewMatrix(glm::inverse(transform));
	}
}
