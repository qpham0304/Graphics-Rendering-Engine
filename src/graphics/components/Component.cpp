#include "headers/Component.h"

Component::Component() {
	modelMatrix = glm::mat4(1.0f);
	isSelected = false;
	showAxis = false;
}


void Component::setUniform()
{
	shaderProgram_ptr->Activate();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram_ptr->ID, "matrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix));

}

void Component::render(Camera& camera, const Light& light)
{
	shaderProgram_ptr->Activate();
	glCullFace(GL_FRONT);
	shaderProgram_ptr->setMat4("lightProjection", light.lightProjection);
	shaderProgram_ptr->setVec4("lightColor", light.color);
	shaderProgram_ptr->setInt("sampleRadius", 2);
	shaderProgram_ptr->setFloat("ambientIntensity", light.ambient);
	shaderProgram_ptr->setBool("hasAnimation", false);
	shaderProgram_ptr->setInt("shadowMap", 2);
	shaderProgram_ptr->setVec3("lightPos", light.position);
	shaderProgram_ptr->setMat4("matrix", modelMatrix);
	glCullFace(GL_BACK);
	// TODO: find a way to separate into 2 batches to set animate boolean in
	// shader to be true or false -> avoid unecessary repeat
	// same thing for shadowmaps shader
}

void Component::scale(glm::vec3& scale)
{
	scaleVector = scale;
	modelMatrix = glm::translate(translateVector);
	modelMatrix = glm::scale(modelMatrix, scale);
}

void Component::translate(glm::vec3& translate)
{
	translateVector = translate;
	modelMatrix = glm::translate(glm::mat4(1.0f), translate);
	modelMatrix = glm::scale(modelMatrix, scaleVector);
}

glm::mat4 Component::getModelMatrix()
{
	return modelMatrix;
}

bool Component::getSelectedState()
{
	return isSelected;
}

bool Component::getShowAxisState()
{
	return showAxis;
}

Component::~Component()
{
	shaderProgram_ptr->Delete();
}

