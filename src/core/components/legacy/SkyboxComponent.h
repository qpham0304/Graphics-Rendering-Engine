#pragma once

#include <Shader.h>
#include <Texture.h>
#include <skybox.h>
#include <camera.h>
#include <algorithm>

class SkyboxComponent
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

	Skybox skybox;
	Shader shaderProgram;

public:
	GLuint VAO;

	SkyboxComponent();
	SkyboxComponent(const char* path);

	void setUniform();
	void updateTexture(const unsigned int& ID);
	unsigned int getTextureID();
	void render(Camera& camera);
	void render(Camera& camera, const unsigned int&);
};

