#pragma once

#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include "Shader.h"

//Camera* currentCamera = nullptr;
static const float DEFAULT_SPEED = 0.1f;
static const float DEFAULT_SENSITIVITY = 0.1f;;
static const float DEFAULT_YAW = -90.0f;
static const float DEFAULT_PITCH = 0.0f;
static const float DEFAULT_FOV = 45.0f;
static const float DEFAULT_NEARPLANE = 0.1f;
static const float DEFAULT_FARPLANE = 1000.0f;

class Camera
{
private:
	glm::vec3 defaultPosition = glm:: vec3(0.0f, 0.0f, 3.0f);
	glm::vec3 defaultOrientation = glm::vec3(0.01f, 0.0f, -1.0f);
	glm::vec3 defaultUp = glm::vec3(0.0f, 1.0f, 0.0f);

	glm::vec3 position;
	glm::vec3 orientation;
	glm::vec3 up = defaultUp;
	glm::vec3 right;
	glm::vec3 rayDirection;

	bool firstClick = true;
	bool shiftPressed = false;
	bool firstInit = false;
	bool cameraMove = false;

	int width;
	int height;
	float lastX;
	float lastY;

	float speedMultiplier = 1;
	float speed = DEFAULT_SPEED;
	float sensitivity = DEFAULT_SENSITIVITY;
	float yaw = DEFAULT_YAW;
	float pitch = DEFAULT_PITCH;
	float nearPlane = DEFAULT_NEARPLANE;
	float farPlane = DEFAULT_FARPLANE;

	float deltaTime = 0.0f;	// Time between current frame and last frame
	float lastFrame = 0.0f; // Time of last frame

	void ReCalculateView();
	void ReCalculateProjection();

	float fov = DEFAULT_FOV;
	glm::mat4 view;
	glm::mat4 inView;
	glm::mat4 projection;
	glm::mat4 inProjection;

public:
	glm::mat4 mvp;

	Camera(unsigned int width, unsigned int height, glm::vec3 position, glm::vec3 orientation);
	Camera(unsigned int width, unsigned int height, glm::vec3 position);
	glm::vec3 getPosition();
	glm::vec3 getOrientation();
	glm::mat4 getViewMatrix();
	glm::mat4 getInViewMatrix();
	glm::mat4 getProjectionMatrix();
	glm::mat4 getInProjectionMatrix();
	glm::mat4 getMVP();

	float getFOV();
	int getViewWidth();
	int getViewHeight();
	bool isMoving();
	float getDeltaTime();
	
	void cameraViewUpdate();
	void updateViewResize(int width, int height);
	void processInput(GLFWwindow* window);
	bool processKeyboard(GLFWwindow* window);
	bool processMouse(GLFWwindow* window);
	void resetCamera();
	void setCameraSpeed(int speedMultiplier);

	//TODO: refactor by moving these to global
	// create a singleton camera that control and manage the view
	// swap to other camera on selected
	void mouseControl(GLFWwindow* window);
	void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

};

