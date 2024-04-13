#pragma once

#include <iostream>
#include <Shader.h>
#include <Texture.h>
#include <skybox.h>
#include <camera.h>
#include "./Component.h"

class SkyboxComponent : Component
{
public:
	GLuint VAO;
	std::unique_ptr<Skybox> skybox;

	SkyboxComponent();

	void setUniform();
	void render(Camera& camera);
};

