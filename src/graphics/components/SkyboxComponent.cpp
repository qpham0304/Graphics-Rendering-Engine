#include "headers/SkyboxComponent.h"

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
	skybox.reset(new Skybox(faces));
	shaderProgram_ptr.reset(new Shader("Shaders/skybox.vert", "Shaders/skybox.frag"));

	setup();
}

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

	skybox.reset(new Skybox(faces));
	shaderProgram_ptr.reset(new Shader("Shaders/skybox.vert", "Shaders/skybox.frag"));
	setup();
}

void SkyboxComponent::setUniform()
{
	shaderProgram_ptr->Activate();
	glUniform1i(glGetUniformLocation(shaderProgram_ptr->ID, "skybox"), 0);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram_ptr->ID, "matrix"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
}

// should draw skybox last
void SkyboxComponent::render(Camera& camera)
{
	glm::mat4 projection = camera.getProjectionMatrix();
	glm::mat4 viewMatrix = glm::mat4(glm::mat3(camera.getViewMatrix()));	 // remove translation from the view matrix
	
	// change depth function so depth test passes when values are equal to depth buffer's content
	glDepthFunc(GL_LEQUAL);				
	shaderProgram_ptr->Activate();
	shaderProgram_ptr->setMat4("view", viewMatrix);
	shaderProgram_ptr->setMat4("projection", projection);

	// skybox cube
	glBindVertexArray(VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->ID);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS); // set depth function back to default
}
