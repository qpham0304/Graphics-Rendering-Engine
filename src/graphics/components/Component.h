#pragma once

#include <memory>
#include <Shader.h>
#include <camera.h>

class Component
{
private:

public:
	GLuint VAO;
	bool selected;
	bool showAxis;
	std::unique_ptr<Shader> shaderProgram_ptr;
	glm::mat4 modelMatrix;

	Component();

	void setUniform();
	void render(Camera& camera);
	void scale(glm::mat4 matrix);
	void rotate(glm::mat4 matrix);
	void translate(glm::mat4 matrix);


	~Component();
};

