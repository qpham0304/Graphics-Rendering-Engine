#include "CameraComponent.h"

CameraComponent::CameraComponent(int width, int height, glm::vec3 position, glm::vec3 orientation)
{
    camera.init(width, height, position, orientation);
}

glm::mat4 CameraComponent::projection()
{
    return camera.getProjectionMatrix();
}
