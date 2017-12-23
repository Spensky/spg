//
//  Camera.cpp
//  Lab5
//
//  Created by CGIS on 28/10/2016.
//  Copyright © 2016 CGIS. All rights reserved.
//

#include "Camera.hpp"
#include <stdio.h>
namespace gps {
    
    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget)
    {
        this->cameraPosition = cameraPosition;
        this->cameraTarget = cameraTarget;
        this->cameraDirection = glm::normalize(cameraTarget - cameraPosition);
		this->Upvector = glm::vec3(0.0f, 1.0f, 0.0f);
        this->cameraRightDirection = glm::normalize(glm::cross(this->cameraDirection, this->Upvector));
		this->ROTATIONAL_SPEED = 0.0005f;
	}
    
    glm::mat4 Camera::getViewMatrix()
    {
        return glm::lookAt(cameraPosition, cameraPosition + cameraDirection , this->Upvector);
    }
    
    void Camera::move(MOVE_DIRECTION direction, float speed)
    {
        switch (direction) {
            case MOVE_FORWARD:
                cameraPosition += cameraDirection * speed;
                break;
                
            case MOVE_BACKWARD:
                cameraPosition -= cameraDirection * speed;
                break;
                
            case MOVE_RIGHT:
                cameraPosition += cameraRightDirection * speed;
                break;
                
            case MOVE_LEFT:
                cameraPosition -= cameraRightDirection * speed;
                break;
        }
    }
    
	void Camera::keyboardCallback(GLFWwindow* glWindow, int key, int scancode, int action, int mode) {
		
		if (glfwGetKey(glWindow, GLFW_KEY_W)) {
			move(gps::MOVE_FORWARD, 0.5f);
		}

		if (glfwGetKey(glWindow, GLFW_KEY_A)) {
			move(gps::MOVE_LEFT, 0.5f);
		}

		if (glfwGetKey(glWindow, GLFW_KEY_D)) {
			move(gps::MOVE_RIGHT, 0.5f);
		}

		if (glfwGetKey(glWindow, GLFW_KEY_S)) {
			move(gps::MOVE_BACKWARD, 0.5f);
		}
	}

	glm::vec2 oldMousePos = glm::vec2(0.0f, 0.0f);

	glm::vec3 Camera::getCameraTarget() {
		glm::vec3 Target = this->cameraTarget;
		return Target;
	}

	void Camera::mouseCallback(GLFWwindow* window, double xpos, double ypos) {
		//printf("X=%d, Y=%d\n",xpos,ypos);
		
		glm::vec2 mousePos = glm::vec2(xpos, ypos);
		glm::vec2 mouseDelta = mousePos - oldMousePos;
		//printf("\n%d -- %d\n", mouseDelta.x, mouseDelta.y);
		cameraDirection = glm::mat3(glm::rotate(-mouseDelta.x * ROTATIONAL_SPEED, this->Upvector)) * this->cameraDirection;
		cameraDirection = glm::mat3(glm::rotate(-mouseDelta.y * ROTATIONAL_SPEED, this->cameraRightDirection)) * this->cameraDirection;
		this->cameraRightDirection = glm::normalize(glm::cross(cameraDirection, this->Upvector));
		oldMousePos = mousePos;
	}

}
