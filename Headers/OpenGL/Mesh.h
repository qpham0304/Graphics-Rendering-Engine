#pragma once
#include <glad/glad.h>
#include <string>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <vector>
#include "Shader.h"
#include "Texture.h"
#include "camera.h"

#define MAX_BONE_INFLUENCE 4

struct Vertex {
	glm::vec3 positions;
	glm::vec3 color;
	glm::vec2 texCoords;
	glm::vec3 normal;
	glm::vec3 tangent;
	glm::vec3 bitangent;
	
	int m_BoneIDs[MAX_BONE_INFLUENCE];		//bone indexes which will influence this vertex
	float m_Weights[MAX_BONE_INFLUENCE];	//weights from each bone
};

class Mesh
{
private:
	unsigned int VAO, VBO, EBO;
	int numVertices = 0;

	void setup();

public:
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture> textures;

	int getNumVertices();
	Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures);

	void Draw(Shader& shader);

};

