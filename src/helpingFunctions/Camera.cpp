#include "Camera.h"
#include <GLFW/glfw3.h>

Camera::Camera(glm::vec3 position) {
    cameraPosition = position;
    cameraFront = glm::vec3(1.0f, 0.0f, 0.0f);
    worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    yaw = -89.0f;
    pitch = 0.0f;
    spead = 2.5f;
    updateCameraVectors();
}

void Camera::updateCameraDirection(double xoffset, double yoffset) {
    // xoffset *= 0.1f;
    // yoffset *= 0.1f;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch >= 89.0f) {
        pitch = 89.0f;
    }
    if (pitch <= -89.0f) {
        pitch = -89.0f;
    }

    updateCameraVectors();
}

void Camera::updateCameraPosition(CameraDireaction direction, float deltaTime) {
    float velocity = (float)spead * deltaTime;
    
    switch (direction){
    case CameraDireaction::FORWARD:
    cameraPosition += cameraFront * velocity;
        //cameraPosition -= cameraUp * velocity;
        break;
    case CameraDireaction::BACKWARD:
    cameraPosition -= cameraFront * velocity;
        //cameraPosition += cameraUp * velocity;
        break;
    case CameraDireaction::LEFT:
        cameraPosition -= cameraRight * velocity;
        break;
    case CameraDireaction::RIGHT:
        cameraPosition += cameraRight * velocity;
        break;
    case CameraDireaction::UP:
        cameraPosition += cameraUp * velocity;
        break;
    case CameraDireaction::DOWN:
        cameraPosition -= cameraUp * velocity;
        break;
    default:
        break;
    }
}
//(transform vertices from world space to camera space)
glm::mat4 Camera::getViewMatrix() {
    //return glm::lookAt(cameraPosition, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    return glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);
}
//(recalculates the camera's front, right, and up vectors based on the current yaw and pitch angles)
void Camera::updateCameraVectors(){
    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    cameraFront = glm::normalize(direction);
    cameraRight = glm::normalize(glm::cross(cameraFront, worldUp));//
    cameraUp = glm::normalize(glm::cross(cameraRight, cameraFront));
}
