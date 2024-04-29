#include "headers/PlaneComponent.h"

void PlaneComponent::setup() {
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

	float cubeVertices[] = {
		// Position               // Normal
		1.000000f, 1.000000f, -1.000000f,  -0.0000f, 1.0000f, -0.0000f,  // Vertex 0
		1.000000f, -1.000000f, -1.000000f, -0.0000f, 1.0000f, -0.0000f,  // Vertex 1
		1.000000f, 1.000000f, 1.000000f,   -0.0000f, 1.0000f, -0.0000f,  // Vertex 2
		1.000000f, -1.000000f, 1.000000f,  -0.0000f, 1.0000f, -0.0000f,  // Vertex 3
		-1.000000f, 1.000000f, -1.000000f, -0.0000f, -0.0000f, 1.0000f,  // Vertex 4
		-1.000000f, -1.000000f, -1.000000f,-0.0000f, -0.0000f, 1.0000f,  // Vertex 5
		-1.000000f, 1.000000f, 1.000000f,  -0.0000f, -0.0000f, 1.0000f,  // Vertex 6
		-1.000000f, -1.000000f, 1.000000f, -0.0000f, -0.0000f, 1.0000f   // Vertex 7
	};

	// Cube indices (faces)
	std::vector<GLuint> cubeIndices = {
		0, 1, 3, // Front face
		4, 5, 7, // Back face
		6, 7, 1, // Left face
		2, 3, 5, // Right face
		2, 6, 4, // Top face
		0, 1, 5  // Bottom face
	};

	//std::vector<Vertex> vertices;
	//std::cout << "size of cube vertices" << sizeof(cubeVertices) / sizeof(float) << "\n";
	//for (int i = 0; sizeof(cubeVertices) / sizeof(float); i += 6) {
	//	Vertex vertex;
	//	vertex.positions = glm::vec3(cubeVertices[i], cubeVertices[i + 1], cubeVertices[i + 2]);
	//	vertex.color = glm::vec3(1.0f, 1.0f, 1.0f);
	//	vertex.texCoords = glm::vec2(1.0, 0.0);
	//	vertex.normal = glm::vec3(cubeVertices[i + 3], cubeVertices[i + 4], cubeVertices[i + 5]);
	//}

	//planeTextures.push_back(Texture("Textures/planks.png", "diffuse", GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE));
	//planeTextures.push_back(Texture("Textures/planksSpec.png", "diffuse", GL_TEXTURE1, GL_RED, GL_UNSIGNED_BYTE));
	//plane_ptr.reset(new Mesh(planeVertices, planeIndices, planeTextures));

	plane_ptr.reset(new Model("Models/cube/cube-notex.gltf"));
	shaderProgram_ptr.reset(new Shader("Shaders/default.vert", "Shaders/default.frag"));
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

void PlaneComponent::renderShadow(Shader& shader)
{
	Component::renderShadow(shader);
	plane_ptr->Draw(shader);
}

void PlaneComponent::render(Camera& camera, const Light& light)
{
	Component::render(camera, light);
	plane_ptr->Draw(*shaderProgram_ptr);

}