#pragma once

#include <iostream>
#include <Shader.h>
#include <Texture.h>
#include <skybox.h>
#include <camera.h>
#include "./Component.h"
#include <algorithm>

class SkyboxComponent : Component
{
private:
	void setup();
	std::vector<std::string> faces = {	// default skybox textures
		"Textures/skybox/right.jpg",
		"Textures/skybox/left.jpg",
		"Textures/skybox/top.jpg",
		"Textures/skybox/bottom.jpg",
		"Textures/skybox/front.jpg",
		"Textures/skybox/back.jpg"
	};


public:
	GLuint VAO;
	std::unique_ptr<Skybox> skybox;

	SkyboxComponent();
	SkyboxComponent(const char* path);

	void setUniform();
	void render(Camera& camera);
};

