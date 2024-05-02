#include "headers/Component.h"

Component::Component() {
	modelMatrix = glm::mat4(1.0f);
	selected = false;
	showAxis = false;
	hasAnimation = false;
	id = Utils::uuid::get_uuid();
	name = id;

	// list of shader that implemented with their uniform
	// allow gui to control them
	attributes["default"] = {""};
}

void Component::setUniform()
{
	shaderProgram_ptr->Activate();
	shaderProgram_ptr->setMat4("matrix", modelMatrix);
}

Component::Component(const char* path)
{
	modelMatrix = glm::mat4(1.0f);
	selected = false;
	showAxis = false;
	hasAnimation = false;
	id = Utils::uuid::get_uuid();
	name = id;

	model_ptr.reset(new Model(path));
	shaderProgram_ptr.reset(new Shader("Shaders/default.vert", "Shaders/default.frag"));
	const char* fileName = std::strrchr(path, '/');
	name = fileName + 1;
}

void Component::render(Camera& camera, const Light& light)
{
	shaderProgram_ptr->Activate();
	shaderProgram_ptr->setMat4("lightProjection", light.mvp);
	shaderProgram_ptr->setVec4("lightColor", light.color);
	shaderProgram_ptr->setInt("sampleRadius", light.sampleRadius);
	shaderProgram_ptr->setFloat("ambientIntensity", light.ambient);
	shaderProgram_ptr->setBool("hasAnimation", hasAnimation);
	shaderProgram_ptr->setInt("shadowMap", 2);	// texture unit slot 2
	shaderProgram_ptr->setVec3("lightPos", light.position);
	shaderProgram_ptr->setVec3("camPos", camera.getPosition());
	shaderProgram_ptr->setMat4("matrix", modelMatrix);
	shaderProgram_ptr->setMat4("mvp", camera.getMVP());
	// TODO: find a way to separate into 2 batches to set animate boolean in
	// shader to be true or false -> avoid unecessary repeat
	// same thing for shadowmaps shader
	countVertices = model_ptr->getNumVertices();
	if (hasAnimation) {
		auto aru_transforms = animator_ptr->GetFinalBoneMatrices();
		for (int i = 0; i < aru_transforms.size(); ++i)
			shaderProgram_ptr->setMat4("finalBonesMatrices[" + std::to_string(i) + "]", aru_transforms[i]);
		model_ptr->Draw(*shaderProgram_ptr);
	}
	else
		model_ptr->Draw(*shaderProgram_ptr);
}

void Component::renderShadow(Shader& shader)
{
	shader.setMat4("matrix", modelMatrix);
	shader.setBool("hasAnimation", hasAnimation);
	if (hasAnimation) {
		auto aru_transforms_shadow = animator_ptr->GetFinalBoneMatrices();
		for (int i = 0; i < aru_transforms_shadow.size(); ++i)
			shader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", aru_transforms_shadow[i]);
		model_ptr->Draw(shader);
	}
	else
		model_ptr->Draw(shader);
}

void Component::loadAnimation(const char* path)
{
	hasAnimation = true;
	animation_ptr.reset(new Animation(path, model_ptr.get()));
	animator_ptr.reset(new Animator(animation_ptr.get()));
}

void Component::updateAnimation(float deltaTime)
{
	this->deltaTime = deltaTime;
	animator_ptr->UpdateAnimation(deltaTime);
}

void Component::translate(glm::vec3& translate)
{
	translateVector = translate;
	modelMatrix = glm::translate(glm::mat4(1.0f), translate);
	rotate(rotationVector);
	modelMatrix = glm::scale(modelMatrix, scaleVector);
}

bool Component::canAnimate()
{
	return hasAnimation;
}

glm::mat4  Component::getTransform()
{
	//glm::mat4 translateMatrix = glm::translate(glm::mat4(1.0f), translateVector);
	//glm::mat4 rotationMatrix = glm::toMat4(glm::quat(rotationVector));
	//glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), scaleVector);
	//modelMatrix = translateMatrix * rotationMatrix * scaleMatrix;

	modelMatrix = glm::translate(glm::mat4(1.0f), translateVector);
	glm::mat4 rot = glm::toMat4(glm::quat(rotationVector));
	modelMatrix *= rot;
	modelMatrix = glm::scale(modelMatrix, scaleVector);

	return modelMatrix;
}

void Component::rotate(glm::vec3& rotate)
{	
	//TODO: broken if used with slider but guizmo works fine, fix if have time
	//rotationVector = rotate;
	modelMatrix = glm::translate(glm::mat4(1.0f), translateVector);
	glm::mat4 rot = glm::toMat4(glm::quat(rotationVector));
	modelMatrix *= rot;
	modelMatrix = glm::scale(modelMatrix, scaleVector);
}

void Component::scale(glm::vec3& scale)
{
	scaleVector = scale;
	modelMatrix = glm::translate(translateVector);
	rotate(rotationVector);
	modelMatrix = glm::scale(modelMatrix, scale);
}

glm::mat4 Component::getModelMatrix()
{
	return modelMatrix;
}

bool Component::getShowAxisState()
{
	return showAxis;
}

std::vector<std::string> Component::uniforms()
{
	//TODO: conditional to list the uniforms that can be altered
	//edit*: nvm, just use a map to map with the shader type 
	//if no match throw error or use a default one
	return {};
}

void Component::select()
{
	selected = true;
}

void Component::unSelect()
{
	selected = false;
}

int Component::getNumVertices()
{
	return countVertices;
}

bool Component::isSelected()
{
	return selected;
}

std::string Component::getID()
{
	return id;
}

std::string Component::getName()
{
	return name;
}

void Component::swapShader(Shader& shader)
{
	if(shaderProgram_ptr->type != shader.type)
		shaderProgram_ptr.reset(new Shader(shader));
}