#include "SkyboxComponent.h"

void SkyboxComponent::setup()
{
	const float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	//// skybox VAO
	GLuint VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
}

SkyboxComponent::SkyboxComponent()
{
	skybox.init(faces);
	shaderProgram.Init("Shaders/skybox.vert", "Shaders/skybox.frag");

	setup();
}

// receive path to the skybox folder and process all six files at once
SkyboxComponent::SkyboxComponent(const char* path)
{

	auto replacePath = [path](std::string& str) {
		size_t pos = str.find_last_of('/');
		if (pos != std::string::npos) {
			std::string filename = str.substr(pos + 1); // Extract filename
			str = std::string(path) + "/" + filename; // Replace path with "NewPath/"
		}
	};
	std::for_each(faces.begin(), faces.end(), replacePath);

	skybox.init(faces);
	shaderProgram.Init("Shaders/skybox.vert", "Shaders/skybox.frag");
	setup();
}

void SkyboxComponent::setUniform()
{
	shaderProgram.Activate();
	glUniform1i(glGetUniformLocation(shaderProgram.ID, "skybox"), 0);
	shaderProgram.setMat4("matrix", glm::mat4(1.0f));
}

void SkyboxComponent::updateTexture(const unsigned int& id)
{
	skybox.updateTexture(id);

}

unsigned int SkyboxComponent::getTextureID()
{
	return skybox.textureID();
}

void SkyboxComponent::render(Camera& camera)
{
	glm::mat4 projection = camera.getProjectionMatrix();
	glm::mat4 viewMatrix = glm::mat4(glm::mat3(camera.getViewMatrix()));	 // remove translation from the view matrix
	
	// change depth function so depth test passes when values are equal to depth buffer's content
	glDepthFunc(GL_LEQUAL);				
	shaderProgram.Activate();
	shaderProgram.setMat4("view", viewMatrix);
	shaderProgram.setMat4("projection", projection);

	// skybox cube
	glBindVertexArray(VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.textureID());
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS);
}

void SkyboxComponent::render(Camera& camera, const unsigned int& ID)
{
	glm::mat4 projection = camera.getProjectionMatrix();
	glm::mat4 viewMatrix = glm::mat4(glm::mat3(camera.getViewMatrix()));	 // remove translation from the view matrix

	// change depth function so depth test passes when values are equal to depth buffer's content
	glDepthFunc(GL_LEQUAL);
	shaderProgram.Activate();
	shaderProgram.setMat4("view", viewMatrix);
	shaderProgram.setMat4("projection", projection);

	// skybox cube
	glBindVertexArray(VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, ID);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS); // set depth function back to default
}
