#include "camera.h"

Camera::Camera(float width, float height, glm::vec3 position)
{
	this->width = width;
	this->height = height;
	this->position = position;
	this->defaultPosition = position;
	this->lastX = width / 2;
	this->lastY = height / 2;
}


void Camera::cameraViewUpdate()
{

	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);

	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
	speed = 2.5f * deltaTime;

	view = glm::lookAt(position, position + orientation, up);
	projection = glm::perspective(glm::radians(fov), (float)width / (float)height, nearPlane, farPlane);
	mvp = projection * view;
}


void Camera::cameraViewObject(GLuint shaderID, const char* uniform)
{
	GLuint uniformLocation = glGetUniformLocation(shaderID, uniform);
	glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(mvp));
}

void Camera::processKeyboard(GLFWwindow* window) {
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	shiftPressed = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS
		|| glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		position += speed * orientation;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		position -= speed * orientation;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		position -= glm::normalize(glm::cross(orientation, up)) * speed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		position += glm::normalize(glm::cross(orientation, up)) * speed;
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !shiftPressed)
		position += glm::normalize(up) * speed;
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && shiftPressed)
		position -= glm::normalize(up) * speed;
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
		resetCamera();
}

Camera* instance = nullptr;

void mousecb(GLFWwindow* window, double xpos, double ypos) {
	instance->mouse_callback(window, xpos, ypos);
}

void scrollcb(GLFWwindow* window, double xpos, double ypos) {
	instance->scroll_callback(window, xpos, ypos);
}

void Camera::processMouse(GLFWwindow* window) {
	
	instance = this;

	glfwSetScrollCallback(window, scrollcb);
	//glfwSetCursorPosCallback(window, mousecb);

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		mouseControl(window);
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
	{
		// Unhides cursor since camera is not looking around anymore
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		// Makes sure the next time the camera looks around it doesn't jump
		firstClick = true;
	}

}

void Camera::resetCamera()
{
	position = defaultPosition;
	orientation = defaultOrientation;
	speed = DEFAULT_SPEED;
	mouseSpeed = DEFAULT_MOUSESPEED;
	sensitivity = DEFAULT_SENSITIVITY;
	yaw = DEFAULT_YAW;
	pitch = DEFAULT_PITCH;
	fov = DEFAULT_FOV;
	nearPlane = DEFAULT_NEARPLANE;
	farPlane = DEFAULT_FARPLANE;
}


void Camera::processInput(GLFWwindow* window)
{
	processKeyboard(window);
	processMouse(window);
}

void Camera::mouseControl(GLFWwindow* window)
{
	double x;
	double y;
	glfwGetCursorPos(window, &x, &y);

	float xpos = static_cast<float>(x);
	float ypos = static_cast<float>(y);

	if (firstClick)
	{
		lastX = xpos;
		lastY = ypos;
		firstClick = false;
	}

	// Calculate change in mouse cursor position
	float xOffset = xpos - lastX;
	float yOffset = lastY - ypos; // Reversed since y-coordinates go from bottom to top

	// Update last mouse cursor position
	lastX = xpos;
	lastY = ypos;

	// Apply changes to camera orientation
	xOffset *= sensitivity;
	yOffset *= sensitivity;

	// Update camera orientation based on mouse movement
	// Example: Adjust yaw and pitch of the camera
	yaw += xOffset;
	pitch += yOffset;

	// Clamp pitch to prevent camera flipping
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	// Update camera direction
	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	orientation = glm::normalize(front);
}

void Camera::mouse_callback(GLFWwindow* window, double x, double y)
{

	float xpos = static_cast<float>(x);
	float ypos = static_cast<float>(y);

	if (firstClick)
	{
		glfwSetCursorPos(window, width / 2, height / 2);
		lastX = xpos;
		lastY = ypos;
		firstClick = false;
	}

	// Calculate change in mouse cursor position
	float xOffset = xpos - lastX;
	float yOffset = lastY - ypos; // Reversed since y-coordinates go from bottom to top

	// Update last mouse cursor position
	lastX = xpos;
	lastY = ypos;

	// Apply changes to camera orientation
	xOffset *= sensitivity;
	yOffset *= sensitivity;

	// Update camera orientation based on mouse movement
	// Example: Adjust yaw and pitch of the camera
	yaw += xOffset;
	pitch += yOffset;

	// Clamp pitch to prevent camera flipping
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	// Update camera direction
	orientation.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	orientation.y = sin(glm::radians(pitch));
	orientation.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	orientation = glm::normalize(orientation);

}

void Camera::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	fov -= (float)yoffset;
	if (fov < 1.0f)
		fov = 1.0f;
	if (fov > 45.0f)
		fov = 45.0f;
}
