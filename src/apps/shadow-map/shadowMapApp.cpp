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
#include <skybox.h>


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processProgramInput(GLFWwindow* window);
void Rotate(glm::mat4 matrix, Shader& shader);
static const std::string DIR = "";
static const unsigned width = 1024;
static const unsigned height = 728;
float lastFrame = 0;
unsigned int frameCounter = 0;
float rotationAngle = 0;
float angle = 0.0f;
float radius = 1.0f;
float angularSpeed = 0.01f;
Camera* cameraController = nullptr;

// Vertices coordinates
std::vector<Vertex> vertices =
{
	Vertex{glm::vec3(-0.5f, 0.0f,  0.5f),glm::vec3(0.83f, 0.70f, 0.44f),glm::vec2(0.0f, 0.0f),glm::vec3(0.0f, -1.0f, 0.0f)}, // Bottom side
	Vertex{glm::vec3(-0.5f, 0.0f, -0.5f),glm::vec3(0.83f, 0.70f, 0.44f),glm::vec2(0.0f, 5.0f),glm::vec3(0.0f, -1.0f, 0.0f)}, // Bottom side
	Vertex{glm::vec3(0.5f, 0.0f, -0.5f),glm::vec3(0.83f, 0.70f, 0.44f),glm::vec2(5.0f, 5.0f),glm::vec3(0.0f, -1.0f, 0.0f)}, // Bottom side
	Vertex{glm::vec3(0.5f, 0.0f,  0.5f),glm::vec3(0.83f, 0.70f, 0.44f),glm::vec2(5.0f, 0.0f),glm::vec3(0.0f, -1.0f, 0.0f)}, // Bottom side
	Vertex{glm::vec3(-0.5f, 0.0f,  0.5f),glm::vec3(0.83f, 0.70f, 0.44f),glm::vec2(0.0f, 0.0f),glm::vec3(-0.8f, 0.5f,  0.0f)}, // Left Side
	Vertex{glm::vec3(-0.5f, 0.0f, -0.5f),glm::vec3(0.83f, 0.70f, 0.44f),glm::vec2(5.0f, 0.0f),glm::vec3(-0.8f, 0.5f,  0.0f)}, // Left Side
	Vertex{glm::vec3(0.0f, 0.8f,  0.0f),glm::vec3(0.92f, 0.86f, 0.76f),glm::vec2(2.5f, 5.0f),glm::vec3(-0.8f, 0.5f,  0.0f)}, // Left Side
	Vertex{glm::vec3(-0.5f, 0.0f, -0.5f),glm::vec3(0.83f, 0.70f, 0.44f),glm::vec2(5.0f, 0.0f),glm::vec3(0.0f, 0.5f, -0.8f)}, // Non-facing side
	Vertex{glm::vec3(0.5f, 0.0f, -0.5f),glm::vec3(0.83f, 0.70f, 0.44f),glm::vec2(0.0f, 0.0f),glm::vec3(0.0f, 0.5f, -0.8f)}, // Non-facing side
	Vertex{glm::vec3(0.0f, 0.8f,  0.0f),glm::vec3(0.92f, 0.86f, 0.76f),glm::vec2(2.5f, 5.0f),glm::vec3(0.0f, 0.5f, -0.8f)}, // Non-facing side
	Vertex{glm::vec3(0.5f, 0.0f, -0.5f),glm::vec3(0.83f, 0.70f, 0.44f),glm::vec2(0.0f, 0.0f),glm::vec3(0.8f, 0.5f,  0.0f)}, // Right side
	Vertex{glm::vec3(0.5f, 0.0f,  0.5f),glm::vec3(0.83f, 0.70f, 0.44f),glm::vec2(5.0f, 0.0f),glm::vec3(0.8f, 0.5f,  0.0f)}, // Right side
	Vertex{glm::vec3(0.0f, 0.8f,  0.0f),glm::vec3(0.92f, 0.86f, 0.76f),glm::vec2(2.5f, 5.0f),glm::vec3(0.8f, 0.5f,  0.0f)}, // Right side
	Vertex{glm::vec3(0.5f, 0.0f,  0.5f),glm::vec3(0.83f, 0.70f, 0.44f),glm::vec2(5.0f, 0.0f),glm::vec3(0.0f, 0.5f,  0.8f)}, // Facing side
	Vertex{glm::vec3(-0.5f, 0.0f,  0.5f),glm::vec3(0.83f, 0.70f, 0.44f),glm::vec2(0.0f, 0.0f),glm::vec3(0.0f, 0.5f,  0.8f)}, // Facing side
	Vertex{glm::vec3(0.0f, 0.8f,  0.0f),glm::vec3(0.92f, 0.86f, 0.76f),glm::vec2(2.5f, 5.0f),glm::vec3(0.0f, 0.5f,  0.8f)}  // Facing side
};

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

struct Material {
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float shininess;
};

struct Light {
	glm::vec3 position;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
};

unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}


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

	glEnable(GL_DEPTH_TEST);

	Camera camera(width, height, glm::vec3(0.0f, 0.5f, 3.0f));
	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightPos = glm::vec3(0.5f, 4.5f, 5.5f);

	//pyramid
	const std::string vertPath = DIR + "Shaders/default.vert";
	const std::string fragPath = DIR + "Shaders/default.frag";
	Shader shaderProgram(vertPath.c_str(), fragPath.c_str());
	std::vector<Texture> textures = {
		Texture("Textures/ice-snow.png", "diffuse", GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE)
	};
	Mesh pyramid(vertices, indices, textures);

	glm::vec3 pyramidPos = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::mat4 pyramidModel = glm::mat4(1.0f);
	pyramidModel = glm::translate(pyramidModel, glm::vec3(-1.0f, 1.0f, 0.0f));
	pyramidModel = glm::translate(pyramidModel, pyramidPos);
	shaderProgram.Activate();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "matrix"), 1, GL_FALSE, glm::value_ptr(pyramidModel));
	glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
	glUniform3f(glGetUniformLocation(shaderProgram.ID, "camPos"), camera.position.x, camera.position.y, camera.position.z);

	//light
	Shader lightShader("Shaders/light.vert", "Shaders/light.frag");
	VAO lightVAO;
	lightVAO.Bind();
	VBO lightVBO(lightVertices, sizeof(lightVertices));
	EBO lightEBO(lightIndices, sizeof(lightIndices));
	lightVAO.LinkAttrib(lightVBO, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
	lightVAO.Unbind();
	lightVBO.Unbind();
	lightEBO.Unbind();

	glm::mat4 lightModel = glm::mat4(1.0f);
	lightModel = glm::translate(lightModel, lightPos);
	lightShader.Activate();
	glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "matrix"), 1, GL_FALSE, glm::value_ptr(lightModel));
	glUniform4f(glGetUniformLocation(lightShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);

	//plane
	std::vector<Texture> planeTextures = {
		Texture("Textures/wood.png", "diffuse", GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE),
		Texture("Textures/planksSpec.png", "diffuse", GL_TEXTURE1, GL_RED, GL_UNSIGNED_BYTE)
	};
	Shader planeShader(vertPath.c_str(), fragPath.c_str());
	Mesh plane(planeVertices, planeIndices, planeTextures);

	glm::mat4 planeModel = glm::mat4(1.0f);
	planeModel = glm::scale(glm::mat4(1.0f), glm::vec3(10.0f, 10.0f, 10.0f));
	planeShader.Activate();
	glUniformMatrix4fv(glGetUniformLocation(planeShader.ID, "matrix"), 1, GL_FALSE, glm::value_ptr(planeModel));
	glUniform4f(glGetUniformLocation(planeShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(planeShader.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
	glUniform3f(glGetUniformLocation(planeShader.ID, "camPos"), camera.position.x, camera.position.y, camera.position.z);


	Shader modelShader("Shaders/default.vert", "Shaders/default.frag");
	glm::mat4 objMatrix = glm::mat4(1.0f);
	objMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.25f, 0.25f, 0.25f));
	objMatrix = glm::translate(objMatrix, glm::vec3(12.0f, 0.0f, 0.0f));
	objMatrix = glm::rotate(objMatrix, 90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	modelShader.Activate();
	glUniformMatrix4fv(glGetUniformLocation(modelShader.ID, "matrix"), 1, GL_FALSE, glm::value_ptr(objMatrix));
	glUniform4f(glGetUniformLocation(modelShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(modelShader.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
	Model ourModel("Models/reimu/reimu.obj");

	//cube
	Shader cubeShader("Shaders/default.vert", "Shaders/default.frag");
	Model cube("Models/cube/cube.obj");
	glm::mat4 cubeMatrix = glm::mat4(1.0f);
	cubeMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.25f, 0.25f, 0.25f));
	cubeMatrix = glm::translate(cubeMatrix, glm::vec3(2.0f, 2.0f, -1.0f));
	cubeShader.Activate();
	glUniformMatrix4fv(glGetUniformLocation(cubeShader.ID, "matrix"), 1, GL_FALSE, glm::value_ptr(cubeMatrix));
	glUniform4f(glGetUniformLocation(cubeShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(cubeShader.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);


	//skybox
	std::vector<std::string> faces = {
		"Textures/skybox/right.jpg",
		"Textures/skybox/left.jpg",
		"Textures/skybox/top.jpg",
		"Textures/skybox/bottom.jpg",
		"Textures/skybox/front.jpg",
		"Textures/skybox/back.jpg"
	};

	std::vector<std::tuple<Mesh, Shader>> meshes = {
		std::make_tuple(pyramid, shaderProgram),
		std::make_tuple(plane, planeShader),
	};

	std::vector<std::tuple<Model, Shader>> models = {
		std::make_tuple(ourModel, modelShader),
		std::make_tuple(cube, cubeShader),
	};

	Shader shadowMapShader("Shaders/shadowMap.vert", "Shaders/shadowMap.frag");
	Shader debugDepthQuad("src/apps/shadow-map/debug.vert", "src/apps/shadow-map/debug.frag");
	
	unsigned int shadowMapFBO;
	glGenFramebuffers(1, &shadowMapFBO);

	unsigned int SHADOW_WIDTH = width * 4, SHADOW_HEIGHT = height * 4;
	unsigned int shadowMap;
	glGenTextures(1, &shadowMap);
	glBindTexture(GL_TEXTURE_2D, shadowMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	
	float clampColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, clampColor);

	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	float near_plane = 1.0f, far_plane = 12.5f;
	//glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
	//glm::mat4 lightView = glm::lookAt(
	//	lightPos,
	//	glm::vec3(0.0f, 0.0f, 0.0f),
	//	glm::vec3(0.0f, 1.0f, 0.0f)
	//);
	//glm::mat4 lightSpaceMatrix = lightProjection * lightView; 

	// Matrices needed for the light's perspective
	//glm::mat4 orthgonalProjection = glm::ortho(-35.0f, 35.0f, -35.0f, 35.0f, 0.1f, 75.0f);
	//glm::mat4 lightView = glm::lookAt(20.0f * lightPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	//glm::mat4 lightProjection = orthgonalProjection * lightView;

	//shadowMapShader.Activate();
	//glUniformMatrix4fv(glGetUniformLocation(shadowMapShader.ID, "lightProjection"), 1, GL_FALSE, glm::value_ptr(lightProjection));


	glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
	glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
	glm::mat4 lightSpaceMatrix = lightSpaceMatrix = lightProjection * lightView;

	debugDepthQuad.Activate();
	debugDepthQuad.setInt("depthMap", 0);

	// Main while loop
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		float timeDiff = currentFrame - lastFrame;
		frameCounter++;

		if (timeDiff >= 1/2) {
			std::string FPS = std::to_string((1.0 / timeDiff) * frameCounter);
			std::string ms = std::to_string((timeDiff / frameCounter) * 1000);
			std::string updatedTitle = "OpenGL Engine - " + FPS + "FPS / " + ms + "ms";
			glfwSetWindowTitle(window, updatedTitle.c_str());
			lastFrame = currentFrame;
			frameCounter = 0;
		}

		bool debug = false;
		// camera inputs
		camera.processInput(window);
		camera.cameraViewUpdate();

		framebuffer_size_callback(window, width, height);
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// render scene from light's point of view
		shadowMapShader.Activate();
		shadowMapShader.setMat4("lightProjection", lightSpaceMatrix);


		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE0);

		shadowMapShader.setMat4("matrix", pyramidModel);
		pyramid.Draw(shadowMapShader, camera);
		shadowMapShader.setMat4("matrix", planeModel);
		plane.Draw(shadowMapShader, camera);
		shadowMapShader.setMat4("matrix", cubeMatrix);
		cube.Draw(shadowMapShader, camera);
		shadowMapShader.setMat4("matrix", objMatrix);
		ourModel.Draw(shadowMapShader, camera);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// reset viewport
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		debugDepthQuad.Activate();
		debugDepthQuad.setFloat("near_plane", near_plane);
		debugDepthQuad.setFloat("far_plane", far_plane);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, shadowMap);
		if(debug) renderQuad();

		//glEnable(GL_DEPTH_TEST);
		//glEnable(GL_CULL_FACE);
		//glCullFace(GL_BACK);
		//glFrontFace(GL_CCW);

		//camera.view = lightView;
		//camera.projection = lightProjection;

		//angle += angularSpeed;
		//lightPos = glm::vec3(
		//	(lightPos.x + radius * cos(angle)) * 0.6,
		//	lightPos.y,
		//	(lightPos.z + radius * sin(angle)) * 0.6
		//);
		//lightModel = glm::translate(lightModel, lightPos);
		//lightShader.Activate();
		//glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "matrix"), 1, GL_FALSE, glm::value_ptr(lightModel));



		glActiveTexture(GL_TEXTURE0 + 2);
		glBindTexture(GL_TEXTURE_2D, shadowMap);

		int count = 0;
		for (const std::tuple<Mesh, Shader>& tuple : meshes) {
			Mesh m = std::get<0>(tuple);
			Shader s = std::get<1>(tuple);
			s.Activate();
			s.setMat4("lightProjection", lightSpaceMatrix);
			glUniform1i(glGetUniformLocation(s.ID, "shadowMap"), 2);
			glUniform3f(glGetUniformLocation(s.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
			m.Draw(s, camera);
			//if (count == 0) {
			//	Rotate(pyramidModel, shaderProgram);
			//}
			count++;
		}

		count = 0;
		for (const std::tuple<Model, Shader>& tuple : models) {
			Model m = std::get<0>(tuple);
			Shader s = std::get<1>(tuple);
			s.Activate();
			s.setMat4("lightProjection", lightSpaceMatrix);
			glUniform1i(glGetUniformLocation(s.ID, "shadowMap"), 2);
			glUniform3f(glGetUniformLocation(s.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
			m.Draw(s, camera);
			count++;
		}

		lightShader.Activate();
		camera.cameraViewObject(lightShader.ID, "mvp");
		lightVAO.Bind();
		glDrawElements(GL_TRIANGLES, sizeof(lightIndices) / sizeof(int), GL_UNSIGNED_INT, 0);

		processProgramInput(window);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

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
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
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

void Rotate(glm::mat4 matrix, Shader& shader) {
	float currentFrame = glfwGetTime();
	float deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	float rotationSpeed = 1;
	rotationAngle += deltaTime * rotationSpeed;

	glm::mat4 rotationMatrix = glm::rotate(matrix, rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "matrix"), 1, GL_FALSE, glm::value_ptr(rotationMatrix));

}


