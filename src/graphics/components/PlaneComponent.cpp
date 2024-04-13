#include "headers/PlaneComponent.h"

std::vector<Vertex> planeVertices =
{					//     COORDINATES     /        COLORS          /    TexCoord   /        NORMALS       //
	Vertex{glm::vec3(-1.0f, 0.0f,  1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f)},
	Vertex{glm::vec3(-1.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f)},
	Vertex{glm::vec3(1.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f)},
	Vertex{glm::vec3(1.0f, 0.0f,  1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f)}
};

std::vector<GLuint> planeIndices = {
	0, 1, 2,
	0, 2, 3
};



void PlaneComponent::setup() {
	planeTextures.push_back(Texture("Textures/planks.png", "diffuse", GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE));
	planeTextures.push_back(Texture("Textures/planksSpec.png", "diffuse", GL_TEXTURE1, GL_RED, GL_UNSIGNED_BYTE));

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, planeVertices.size() * sizeof(Vertex), &planeVertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, planeIndices.size() * sizeof(unsigned int), &planeIndices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));
	glEnableVertexAttribArray(6);
	glVertexAttribIPointer(6, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));
	glEnableVertexAttribArray(7);
	glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));

	glBindVertexArray(0);

	shaderProgram_ptr.reset(new Shader("Shaders/default.vert", "Shaders/default.frag"));
	//plane_ptr.reset(new Mesh(planeVertices, planeIndices, planeTextures));
}

PlaneComponent::PlaneComponent()
{
	setup();
}

PlaneComponent::PlaneComponent(glm::vec3 scale)
{
	setup();
	scaleVector = scale;
	Component::scale(scale);
}


void PlaneComponent::setUniform()
{

}

void PlaneComponent::renderShadow(Shader& shader, Camera& camera)
{
	shader.setMat4("matrix", modelMatrix);
	shader.setBool("hasAnimation", false);
	//plane_ptr->Draw(shader, camera);
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(planeIndices.size()), GL_UNSIGNED_INT, 0);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

void PlaneComponent::render(Camera& camera, Light& light)
{
	Component::render(camera, light);
	//plane_ptr->Draw(*shaderProgram_ptr, camera);

	shaderProgram_ptr->Activate();
	shaderProgram_ptr->setBool("useTexture", false);
	shaderProgram_ptr->setVec3("camPos", camera.position);
	camera.cameraViewObject(shaderProgram_ptr->ID, "mvp");
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(planeIndices.size()), GL_UNSIGNED_INT, 0);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}
