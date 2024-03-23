#pragma once

#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include "Shader.h";
#include "VAO.h";
#include "VBO.h";
#include "EBO.h";
#include "Triangle.h";
#include <memory>

class DrawShape {

private:
	GLuint v;
	std::unique_ptr<Shader> shaderProgram_ptr;

public:

	DrawShape();

	void Draw();
};