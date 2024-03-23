#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/gl.h>
enum class CameraDireaction {
    NONE =0,
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

class Camera {
public:
    glm::vec3 cameraPosition;

    glm::vec3 cameraFront;
    glm::vec3 cameraUp;
    glm::vec3 cameraRight;

    glm::vec3 worldUp;

    float yaw;  // x-axis
    float pitch;    // y-axis
    float spead;

    Camera(glm::vec3 position);
    void updateCameraDirection(double xoffset, double yoffset);
    void updateCameraPosition(CameraDireaction direction, float deltaTime);

    glm::mat4 getViewMatrix();
private:
    void updateCameraVectors();
};
#endif