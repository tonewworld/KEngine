#include "kepch.h"
#include "Camera.h"

namespace KEngine
{
	
	void Camera::Control()
	{
		CalculateViewMatrix();
	}
	void Camera::CalculateViewMatrix()
	{
		
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) 
			* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), glm::vec3(0, 0, 1));
	
		SetViewMatrix(glm::inverse(transform));
	}
}
