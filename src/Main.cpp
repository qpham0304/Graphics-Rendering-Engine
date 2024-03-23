#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <math.h>
#include "shader.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "texture.h"
#include "camera.h"
#include "Triangle.h"
#include "imgui/imgui.h"
#include <vector>
#include <Mesh.h>
#include "model.h"


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processProgramInput(GLFWwindow* window);
static const std::string DIR = "";
static const unsigned width = 1024;
static const unsigned height = 728;
Camera* cameraController = nullptr;

// Vertices coordinates
std::vector<Vertex> vertices =
{					//     COORDINATES     /    TexCoord   /        NORMALS       //
	Vertex{glm::vec3(-0.5f, 0.0f,  0.5f),glm::vec3(0.83f, 0.70f, 0.44f),glm::vec2(0.0f, 0.0f),glm::vec3( 0.0f, -1.0f, 0.0f)}, // Bottom side
	Vertex{glm::vec3(-0.5f, 0.0f, -0.5f),glm::vec3(0.83f, 0.70f, 0.44f),glm::vec2(0.0f, 5.0f),glm::vec3( 0.0f, -1.0f, 0.0f)}, // Bottom side
	Vertex{glm::vec3( 0.5f, 0.0f, -0.5f),glm::vec3(0.83f, 0.70f, 0.44f),glm::vec2(5.0f, 5.0f),glm::vec3( 0.0f, -1.0f, 0.0f)}, // Bottom side
	Vertex{glm::vec3( 0.5f, 0.0f,  0.5f),glm::vec3(0.83f, 0.70f, 0.44f),glm::vec2(5.0f, 0.0f),glm::vec3( 0.0f, -1.0f, 0.0f)}, // Bottom side
	Vertex{glm::vec3(-0.5f, 0.0f,  0.5f),glm::vec3(0.83f, 0.70f, 0.44f),glm::vec2(0.0f, 0.0f),glm::vec3(-0.8f, 0.5f,  0.0f)}, // Left Side
	Vertex{glm::vec3(-0.5f, 0.0f, -0.5f),glm::vec3(0.83f, 0.70f, 0.44f),glm::vec2(5.0f, 0.0f),glm::vec3(-0.8f, 0.5f,  0.0f)}, // Left Side
	Vertex{glm::vec3( 0.0f, 0.8f,  0.0f),glm::vec3(0.92f, 0.86f, 0.76f),glm::vec2(2.5f, 5.0f),glm::vec3(-0.8f, 0.5f,  0.0f)}, // Left Side
	Vertex{glm::vec3(-0.5f, 0.0f, -0.5f),glm::vec3(0.83f, 0.70f, 0.44f),glm::vec2(5.0f, 0.0f),glm::vec3( 0.0f, 0.5f, -0.8f)}, // Non-facing side
	Vertex{glm::vec3( 0.5f, 0.0f, -0.5f),glm::vec3(0.83f, 0.70f, 0.44f),glm::vec2(0.0f, 0.0f),glm::vec3( 0.0f, 0.5f, -0.8f)}, // Non-facing side
	Vertex{glm::vec3( 0.0f, 0.8f,  0.0f),glm::vec3(0.92f, 0.86f, 0.76f),glm::vec2(2.5f, 5.0f),glm::vec3( 0.0f, 0.5f, -0.8f)}, // Non-facing side
	Vertex{glm::vec3( 0.5f, 0.0f, -0.5f),glm::vec3(0.83f, 0.70f, 0.44f),glm::vec2(0.0f, 0.0f),glm::vec3( 0.8f, 0.5f,  0.0f)}, // Right side
	Vertex{glm::vec3( 0.5f, 0.0f,  0.5f),glm::vec3(0.83f, 0.70f, 0.44f),glm::vec2(5.0f, 0.0f),glm::vec3( 0.8f, 0.5f,  0.0f)}, // Right side
	Vertex{glm::vec3( 0.0f, 0.8f,  0.0f),glm::vec3(0.92f, 0.86f, 0.76f),glm::vec2(2.5f, 5.0f),glm::vec3( 0.8f, 0.5f,  0.0f)}, // Right side
	Vertex{glm::vec3( 0.5f, 0.0f,  0.5f),glm::vec3(0.83f, 0.70f, 0.44f),glm::vec2(5.0f, 0.0f),glm::vec3( 0.0f, 0.5f,  0.8f)}, // Facing side
	Vertex{glm::vec3(-0.5f, 0.0f,  0.5f),glm::vec3(0.83f, 0.70f, 0.44f),glm::vec2(0.0f, 0.0f),glm::vec3( 0.0f, 0.5f,  0.8f)}, // Facing side
	Vertex{glm::vec3( 0.0f, 0.8f,  0.0f),glm::vec3(0.92f, 0.86f, 0.76f),glm::vec2(2.5f, 5.0f),glm::vec3( 0.0f, 0.5f,  0.8f)}  // Facing side
};

std::vector<Vertex> planeVertices =
{					//     COORDINATES     /        COLORS          /    TexCoord   /        NORMALS       //
	Vertex{glm::vec3(-1.0f, 0.0f,  1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f)},
	Vertex{glm::vec3(-1.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f)},
	Vertex{glm::vec3( 1.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f)},
	Vertex{glm::vec3( 1.0f, 0.0f,  1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f)}
};

std::vector<GLuint> planeIndices = {
	0, 1, 2,
	0, 2, 3
};

// Indices for vertices order
std::vector<GLuint> indices =
{
	0, 1, 2, // Bottom side
	0, 2, 3, // Bottom side
	4, 6, 5, // Left side
	7, 9, 8, // Non-facing side
	10, 12, 11, // Right side
	13, 15, 14 // Facing side
};

GLfloat lightVertices[] =
{ //     COORDINATES     //
	-0.1f, -0.1f,  0.1f,
	-0.1f, -0.1f, -0.1f,
	 0.1f, -0.1f, -0.1f,
	 0.1f, -0.1f,  0.1f,
	-0.1f,  0.1f,  0.1f,
	-0.1f,  0.1f, -0.1f,
	 0.1f,  0.1f, -0.1f,
	 0.1f,  0.1f,  0.1f
};

GLuint lightIndices[] =
{
	0, 1, 2,
	0, 2, 3,
	0, 4, 7,
	0, 7, 3,
	3, 7, 6,
	3, 6, 2,
	2, 6, 5,
	2, 5, 1,
	1, 5, 4,
	1, 4, 0,
	4, 5, 6,
	4, 6, 7
};


int main() {
	glfwInit();

	// Tell GLFW what version of OpenGL we are using 
	// In this case we are using OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Tell GLFW we are using the CORE profile
	// So that means we only have the modern functions
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(width, height, "Graphic Engine", NULL, NULL);
	// Error check if the window fails to create
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	// Introduce the window into the current context
	glfwMakeContextCurrent(window);

	gladLoadGL();
	glViewport(0, 0, width, height);

	const std::string vertPath = DIR + "Shaders/default.vert";
	const std::string fragPath = DIR + "Shaders/default.frag";
	Shader shaderProgram(vertPath.c_str(), fragPath.c_str());
	std::vector<Texture> textures = {
		Texture("Textures/ice-snow.png", "diffuse", GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE)
	};
	Mesh tri(vertices, indices, textures);


	const std::string lightVertPath = DIR + "Shaders/light.vert";
	const std::string lightFragPath = DIR + "Shaders/light.frag";
	Shader lightShader(lightVertPath.c_str(), lightFragPath.c_str());
	VAO lightVAO;
	lightVAO.Bind();
	VBO lightVBO(lightVertices, sizeof(lightVertices));
	EBO lightEBO(lightIndices, sizeof(lightIndices));
	lightVAO.LinkAttrib(lightVBO, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
	lightVAO.Unbind();
	lightVBO.Unbind();
	lightEBO.Unbind();
	

	std::vector<Texture> planeTextures = {
		Texture("Textures/wood.png", "diffuse", GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE),
		Texture("Textures/planksSpec.png", "diffuse", GL_TEXTURE1, GL_RED, GL_UNSIGNED_BYTE)
	};
	Shader planeShader(vertPath.c_str(), fragPath.c_str());
	Mesh plane(planeVertices, planeIndices, planeTextures);


	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightPos = glm::vec3(0.5f, 0.5f, 0.5f);
	glm::mat4 lightModel = glm::mat4(1.0f);
	lightModel = glm::translate(lightModel, lightPos);

	glm::vec3 pyramidPos = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::mat4 pyramidModel = glm::mat4(1.0f);
	pyramidModel = glm::translate(pyramidModel, pyramidPos);

	glm::mat4 planeModel = glm::mat4(1.0f);
	planeModel = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 2.0f));

	lightShader.Activate();
	glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "matrix"), 1, GL_FALSE, glm::value_ptr(lightModel));
	glUniform4f(glGetUniformLocation(lightShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	
	shaderProgram.Activate();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "matrix"), 1, GL_FALSE, glm::value_ptr(pyramidModel));
	glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

	planeShader.Activate();
	glUniformMatrix4fv(glGetUniformLocation(planeShader.ID, "matrix"), 1, GL_FALSE, glm::value_ptr(planeModel));
	glUniform4f(glGetUniformLocation(planeShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(planeShader.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);


	Shader modelShader("Shaders/model.vert", "Shaders/model.frag");
	Model ourModel("Models/backpack/backpack.obj");


	std::vector<Mesh> meshes = {};

	glEnable(GL_DEPTH_TEST);


	Camera camera(width, height, glm::vec3(0.0f, 0.5f, 2.0f));

	// Main while loop
	while (!glfwWindowShouldClose(window))
	{
		framebuffer_size_callback(window, width, height);
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Handles camera inputs
		camera.processInput(window);
		camera.cameraViewUpdate();

		tri.Draw(shaderProgram, camera);
		plane.Draw(planeShader, camera);
		ourModel.Draw(modelShader, camera);


		lightShader.Activate();
		camera.cameraViewObject(lightShader.ID, "mvp");
		lightVAO.Bind();
		glDrawElements(GL_TRIANGLES, sizeof(lightIndices) / sizeof(int), GL_UNSIGNED_INT, 0);

		processProgramInput(window);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	//shaderProgram.Delete();
	
	lightVAO.Delete();
	lightVBO.Delete();
	lightEBO.Delete();
	lightShader.Delete();

	// Delete window before ending the program
	glfwDestroyWindow(window);
	// Terminate GLFW before ending the program
	glfwTerminate();
	return 0;
}

void processProgramInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}



