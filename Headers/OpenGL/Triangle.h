#pragma once

#include<iostream>
#include<memory>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include "Shader.h";
#include "VAO.h";
#include "VBO.h";
#include "EBO.h";
#include <glm/gtx/transform.hpp>
#include "Texture.h"
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/vector_angle.hpp>


class Triangle
{
private:
	GLfloat* vertices;
	GLuint* indices;
	std::unique_ptr<Shader> shaderProgram_ptr;
	GLuint va;
	std::unique_ptr<Texture> texture_ptr;
	
	int x, y, z;


	float lastFrame = 0.0f;
	float rotationSpeed = 1.0f;
	float rotationAngle = 0.0f;

public:
	GLuint matrixID;
	Triangle(GLfloat vertices[], unsigned int verticesSize, GLuint indices[], unsigned int indicesSize);
	glm::mat4 matrix = glm::mat4(1.0f);

	void Draw();
	void DrawCube();
	void Rotate();
	void Translate(int x, int y, int z);
	Shader getShaderProgram();
};