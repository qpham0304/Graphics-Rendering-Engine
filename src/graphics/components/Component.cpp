#include "Component.h"

Component::Component() {

}


void Component::setUniform()
{
	shaderProgram_ptr->Activate();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram_ptr->ID, "matrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix));

}

void Component::render(Camera& camera)
{

}

Component::~Component()
{

}

