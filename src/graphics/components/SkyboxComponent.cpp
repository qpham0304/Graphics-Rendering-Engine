#include "SkyboxComponent.h"

SkyboxComponent::SkyboxComponent()
{
	float skyboxVertices[] = {
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
	
	//skybox
	std::vector<std::string> faces = {
		"Textures/skybox/right.jpg",
		"Textures/skybox/left.jpg",
		"Textures/skybox/top.jpg",
		"Textures/skybox/bottom.jpg",
		"Textures/skybox/front.jpg",
		"Textures/skybox/back.jpg"
	};

	skybox.reset(new Skybox(faces));
	shaderProgram_ptr.reset(new Shader("Shaders/skybox.vert", "Shaders/skybox.frag"));

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

void SkyboxComponent::setUniform()
{
	shaderProgram_ptr->Activate();
	glUniform1i(glGetUniformLocation(shaderProgram_ptr->ID, "skybox"), 0);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram_ptr->ID, "matrix"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
}

// should draw skybox last
void SkyboxComponent::render(Camera& camera)
{
	glm::mat4 projection = camera.projection;
	glm::mat4 viewMatrix = glm::mat4(glm::mat3(camera.view));	 // remove translation from the view matrix
	
	// change depth function so depth test passes when values are equal to depth buffer's content
	glDepthFunc(GL_LEQUAL);				
	shaderProgram_ptr->Activate();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram_ptr->ID, "view"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram_ptr->ID, "projection"), 1, GL_FALSE, glm::value_ptr(camera.projection));

	// skybox cube
	glBindVertexArray(VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->ID);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS); // set depth function back to default
}
