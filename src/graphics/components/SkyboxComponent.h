#pragma once

#include<iostream>
#include<memory>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include "Shader.h";
#include <glm/gtx/transform.hpp>
#include "Texture.h"
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/vector_angle.hpp>
#include <skybox.h>
#include <camera.h>

class SkyboxComponent
{
public:
	std::unique_ptr<Shader> shaderProgram_ptr;
	std::unique_ptr<Skybox> skybox;
	GLuint skyboxVAO;

	SkyboxComponent();

	void setUniform();
	void render(Camera camera);
};

