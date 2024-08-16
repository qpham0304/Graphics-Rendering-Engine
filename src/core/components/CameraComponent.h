#pragma once
#include <memory>
#include <glm/glm.hpp>
#include "camera.h"

class Camera;

class CameraComponent
{
private:

public:
	Camera camera;

	CameraComponent(int width, int height, glm::vec3 position, glm::vec3 orientation);
	glm::mat4 projection();
	glm::mat4 view();
	glm::mat4 move();
};

