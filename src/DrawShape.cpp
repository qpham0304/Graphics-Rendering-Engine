//#include<iostream>
//#include<glad/glad.h>
//#include<GLFW/glfw3.h>
//#include "Shader.h";
//#include "VAO.h";
//#include "VBO.h";
//#include "EBO.h";
//#include "Triangle.h";
//#include <memory>

#include "Drawshape.h"


DrawShape::DrawShape() {
	// Vertices coordinates
	GLfloat vertices[] =
	{
		-0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f, // Lower left corner
		0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f, // Lower right corner
		0.0f, 0.5f * float(sqrt(3)) * 2 / 3, 0.0f, // Upper corner
		-0.5f / 2, 0.5f * float(sqrt(3)) / 6, 0.0f, // Inner left
		0.5f / 2, 0.5f * float(sqrt(3)) / 6, 0.0f, // Inner right
		0.0f, -0.5f * float(sqrt(3)) / 3, 0.0f // Inner down
	};

	// Indices for vertices order
	GLuint indices[] =
	{
		0, 3, 5, // Lower left triangle
		3, 2, 4, // Lower right triangle
		5, 4, 1 // Upper triangle
	};

	shaderProgram_ptr.reset(new Shader("default.vert", "default.frag"));
	GLuint VAO, VBO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	v = VAO;
}

void DrawShape::Draw() {
	shaderProgram_ptr->Activate();
	// Bind the VAO so OpenGL knows to use it
	glBindVertexArray(v);
	// Draw the triangle using the GL_TRIANGLES primitive
	glDrawArrays(GL_TRIANGLES, 0, 3);
}


