#include "./Component.h"

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
	
	shaderProgram_ptr.reset(new Shader("Shaders/default.vert", "Shaders/default.frag", "Shaders/default.geom"));
	//shaderProgram_ptr.reset(new Shader("Shaders/default-2.vert", "Shaders/default-2.frag"));
	model_ptr.reset(new Model(path));
	const char* fileName = std::strrchr(path, '/');
	name = fileName + 1;
}

Component::Component(Component&& other) noexcept
	: selected(other.selected),
	showAxis(other.showAxis),
	hasAnimation(other.hasAnimation),
	countVertices(other.countVertices),
	deltaTime(other.deltaTime),
	id(std::move(other.id)),
	attributes(std::move(other.attributes)),
	name(std::move(other.name)),
	animation_ptr(std::move(other.animation_ptr)),
	animator_ptr(std::move(other.animator_ptr)),
	shaderProgram_ptr(std::move(other.shaderProgram_ptr)),
	modelMatrix(std::move(other.modelMatrix)),
	normalMatrix(std::move(other.normalMatrix)),
	model_ptr(std::move(other.model_ptr)),
	scaleVector(std::move(other.scaleVector)),
	translateVector(std::move(other.translateVector)),
	rotationVector(std::move(other.rotationVector)),
	material(std::move(other.material)),
	materialPBR(std::move(other.materialPBR))
{
	std::cout << "component " << id << " moved\n";
}

Component& Component::operator=(Component&& other) noexcept
{
	if (this != &other) {
		selected = other.selected;
		showAxis = other.showAxis;
		hasAnimation = other.hasAnimation;
		countVertices = other.countVertices;
		deltaTime = other.deltaTime;
		id = std::move(other.id);
		attributes = std::move(other.attributes);
		name = std::move(other.name);
		animation_ptr = std::move(other.animation_ptr);
		animator_ptr = std::move(other.animator_ptr);
		shaderProgram_ptr = std::move(other.shaderProgram_ptr);
		modelMatrix = std::move(other.modelMatrix);
		normalMatrix = std::move(other.normalMatrix);
		model_ptr = std::move(other.model_ptr);
		scaleVector = std::move(other.scaleVector);
		translateVector = std::move(other.translateVector);
		rotationVector = std::move(other.rotationVector);
		material = std::move(other.material);
		materialPBR = std::move(other.materialPBR);
	}
	std::cout << "component " << other.id << " operator moved\n";
	return *this;
}

void Component::renderPBR(Camera& camera, const Light& light, const UniformProperties& uniforms, const std::vector<Light*> lights)
{
	shaderProgram_ptr->Activate();

	normalMatrix = modelMatrix;
	glm::transpose(glm::inverse(normalMatrix));
	shaderProgram_ptr->setMat3("normalMatrix", normalMatrix);

	shaderProgram_ptr->setMat4("matrix", modelMatrix);
	shaderProgram_ptr->setMat4("mvp", camera.getMVP());
	shaderProgram_ptr->setVec3("camPos", camera.getPosition());
	shaderProgram_ptr->setBool("enableFog", uniforms.enableFog);
	shaderProgram_ptr->setFloat("explodeRadius", uniforms.explodeRadius);
	shaderProgram_ptr->setBool("gamma", uniforms.gammaCorrection);
	shaderProgram_ptr->setInt("shadowMap", 2);	// texture unit slot 2
	shaderProgram_ptr->setBool("hasAnimation", hasAnimation);


	// light properties
	shaderProgram_ptr->setVec3("lightPos", light.position);
	//shaderProgram_ptr->setVec3("light.position", light.position);
	shaderProgram_ptr->setVec4("light.color", light.color);
	shaderProgram_ptr->setVec3("light.ambient", light.ambient);
	shaderProgram_ptr->setVec3("light.diffuse", light.diffuse);
	shaderProgram_ptr->setVec3("light.specular", light.specular);
	shaderProgram_ptr->setMat4("lightMVP", light.mvp);
	shaderProgram_ptr->setInt("light.sampleRadius", light.sampleRadius);

	// animation
	// TODO: find a way to separate into 2 batches to set animate boolean in
	// shader to be true or false -> avoid unecessary repeat
	// same thing for shadowmaps shader
	countVertices = model_ptr->getNumVertices();
	if (hasAnimation) {
		auto aru_transforms = animator_ptr->GetFinalBoneMatrices();
		for (int i = 0; i < aru_transforms.size(); ++i)
			shaderProgram_ptr->setMat4("finalBonesMatrices[" + std::to_string(i) + "]", aru_transforms[i]);
	}

	// material uniform
	shaderProgram_ptr->setVec3("material.albedo", materialPBR.albedo);
	shaderProgram_ptr->setFloat("material.metalic", materialPBR.metalic);
	shaderProgram_ptr->setFloat("material.roughness", materialPBR.roughness);
	shaderProgram_ptr->setFloat("material.ao", materialPBR.ao);

	glm::mat4 model = glm::mat4(1.0f);
	for (unsigned int i = 0; i < lights.size(); ++i)
	{
		glm::vec3 newPos = lights[i]->position + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
		newPos = lights[i]->position;
		shaderProgram_ptr->setVec3("lightPositions[" + std::to_string(i) + "]", newPos);
		shaderProgram_ptr->setVec3("lightColors[" + std::to_string(i) + "]", lights[i]->color);
	}
	model_ptr->Draw(*shaderProgram_ptr);
}

void Component::render(Camera& camera, const Light& light, const UniformProperties& uniforms)
{
	shaderProgram_ptr->Activate();
	normalMatrix = modelMatrix;
	glm::transpose(glm::inverse(normalMatrix));
	shaderProgram_ptr->setMat3("normalMatrix", normalMatrix);
	shaderProgram_ptr->setFloat("far_plane", 50.0);	// TODO: make a constant somewhere for far plane

	shaderProgram_ptr->setMat4("matrix", modelMatrix);
	shaderProgram_ptr->setMat4("mvp", camera.getMVP());

	shaderProgram_ptr->setVec3("camPos", camera.getPosition());
	shaderProgram_ptr->setBool("enableFog", uniforms.enableFog);
	shaderProgram_ptr->setFloat("explodeRadius", uniforms.explodeRadius);
	shaderProgram_ptr->setInt("shadowMap", 2);	// texture unit slot 2
	shaderProgram_ptr->setBool("hasAnimation", hasAnimation);
	shaderProgram_ptr->setInt("shadowMapPoint", 5);

	// material uniform
	shaderProgram_ptr->setVec3("material.diffuse", material.diffuse);
	shaderProgram_ptr->setVec3("material.specular", material.specular);
	shaderProgram_ptr->setFloat("material.shininess", material.shininess);

	// light properties
	shaderProgram_ptr->setVec3("lightPos", light.position);
	//shaderProgram_ptr->setVec3("light.position", light.position);
	shaderProgram_ptr->setVec4("light.color", light.color);
	shaderProgram_ptr->setVec3("light.ambient", light.ambient);
	shaderProgram_ptr->setVec3("light.diffuse", light.diffuse);
	shaderProgram_ptr->setVec3("light.specular", light.specular);
	shaderProgram_ptr->setMat4("lightMVP", light.mvp);
	shaderProgram_ptr->setInt("light.sampleRadius", light.sampleRadius);

	// animation
	// TODO: find a way to separate into 2 batches to set animate boolean in
	// shader to be true or false -> avoid unecessary repeat
	// same thing for shadowmaps shader
	countVertices = model_ptr->getNumVertices();
	if (hasAnimation) {
		auto aru_transforms = animator_ptr->GetFinalBoneMatrices();
		for (int i = 0; i < aru_transforms.size(); ++i)
			shaderProgram_ptr->setMat4("finalBonesMatrices[" + std::to_string(i) + "]", aru_transforms[i]);
	}
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

void Component::translate(glm::vec3 translate)
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

void Component::rotate(glm::vec3 rotate)
{	
	//TODO: broken if used with slider but guizmo works fine, fix if have time
	//rotationVector = rotate;
	modelMatrix = glm::translate(glm::mat4(1.0f), translateVector);
	glm::mat4 rot = glm::toMat4(glm::quat(rotationVector));
	modelMatrix *= rot;
	modelMatrix = glm::scale(modelMatrix, scaleVector);
}

void Component::scale(glm::vec3 scale)
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

glm::mat4 Component::getNormalMatrix()
{
	return normalMatrix;
}

bool Component::getShowAxisState()
{
	return showAxis;
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