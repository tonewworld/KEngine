#include "kepch.h"
#include "Camera.h"
#include "KeyCode.h"
namespace KEngine
{
	
	void Camera::Control(float timeStep)
	{
		/*glm::vec3 temp = glm::vec3(GetPosition());
		if(KEngine::Input::IsKeyPressed(KE_KEY_LEFT)){
			SetPosition(temp.x -= moveSpeed * timeStep);
		}else if(KEngine::Input::IsKeyPressed(KE_KEY_LEFT)){
			SetPosition(temp.x += moveSpeed * timeStep);
		}
		if((KEngine::Input::IsKeyPressed(KE_KEY_UP)){
			SetPosition(temp.y += moveSpeed * timeStep);
		}else if((KEngine::Input::IsKeyPressed(KE_KEY_DOWN)){
			SetPosition(temp.y -= moveSpeed * timeStep);
		}*/


		CalculateViewMatrix();
	}
	void Camera::CalculateViewMatrix()
	{
		
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) 
			* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), glm::vec3(0, 0, 1));
	
		SetViewMatrix(glm::inverse(transform));
	}
}
