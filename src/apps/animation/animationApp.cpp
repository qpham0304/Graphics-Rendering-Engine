#include <iostream>
#include <math.h>
#include "camera.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <vector>
#include <Mesh.h>
#include <model.h>
#include <Animation.h>
#include <Animator.h>
#include "../../graphics/components/headers/SkyboxComponent.h"
#include <chrono>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processProgramInput(GLFWwindow* window);
void Rotate(glm::mat4 matrix, Shader& shader);
static const std::string DIR = "";
unsigned int width = 1024;
unsigned int height = 728;
float lastFrame = 0;
float lf = 0;
float lastTime = 0;
unsigned int frameCounter = 0;
float rotationAngle = 0;
float angle = 0.0f;
float radius = 1.0f;
float angularSpeed = 0.01f;
Camera* cameraController = nullptr;
glm::mat4 tmpMatrix;

Camera camera(width, height, glm::vec3(-6.5f, 3.5f, 8.5f), glm::vec3(0.5, -0.2, -1.0f));
glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
glm::vec3 lightPos = glm::vec3(0.5f, 4.5f, 5.5f);


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

void renderQuad()
{
	unsigned int quadVAO = 0;
	unsigned int quadVBO;
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

void setupDearImGui(GLFWwindow* window, ImGuiIO& io) {
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");
	ImGui::GetStyle().ScaleAllSizes(1);
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


	auto start = std::chrono::high_resolution_clock::now();


	//plane
	std::vector<Texture> planeTextures = {
		Texture("Textures/planks.png", "diffuse", GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE),
		Texture("Textures/planksSpec.png", "diffuse", GL_TEXTURE1, GL_RED, GL_UNSIGNED_BYTE)
	};
	Shader planeShader("Shaders/default.vert", "Shaders/default.frag");
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
	objMatrix = glm::translate(objMatrix, glm::vec3(12.0f, -1.0f, 0.0f));
	modelShader.Activate();
	glUniformMatrix4fv(glGetUniformLocation(modelShader.ID, "matrix"), 1, GL_FALSE, glm::value_ptr(objMatrix));
	glUniform4f(glGetUniformLocation(modelShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(modelShader.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
	glUniform3f(glGetUniformLocation(modelShader.ID, "camPos"), camera.position.x, camera.position.y, camera.position.z);
	Model ourModel("Models/reimu/reimu.obj");


	Shader aruModelShader("Shaders/default.vert", "Shaders/default.frag");
	glm::mat4 aruObjMatrix = glm::mat4(1.0f);

	//Note: Animation doesn't work with scaling matrix right now 
	//so don't scale them out of proportion or shit's gonna get yeet
	aruObjMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
	aruObjMatrix = glm::translate(aruObjMatrix, glm::vec3(-5.0f, 0.25f, 0.0f));
	aruModelShader.Activate();
	glUniformMatrix4fv(glGetUniformLocation(aruModelShader.ID, "matrix"), 1, GL_FALSE, glm::value_ptr(aruObjMatrix));
	glUniform4f(glGetUniformLocation(aruModelShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(aruModelShader.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
	glUniform3f(glGetUniformLocation(aruModelShader.ID, "camPos"), camera.position.x, camera.position.y, camera.position.z);
	Model aruModel("Models/aru/edited/aru_edited.gltf");
	Animation aru_animation("Models/aru/edited/aru_edited.gltf", &aruModel);
	Animator aru_animator(&aru_animation);

	SkyboxComponent skybox;
	skybox.setUniform();

	Shader shadowMapShader("Shaders/shadowMap.vert", "Shaders/shadowMap.frag");
	Shader debugDepthQuad("src/apps/shadow-map/debug.vert", "src/apps/shadow-map/debug.frag");

	unsigned int shadowMapFBO;
	glGenFramebuffers(1, &shadowMapFBO);

	unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
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


	debugDepthQuad.Activate();
	debugDepthQuad.setInt("depthMap", 0);


	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO io;
	setupDearImGui(window, io);

	bool show_demo_window = false;
	bool show_style_editor = false;
	bool show_debug_window = false;
	bool camera_control_enabled = true;
	bool face_culling_enabled = true;
	bool debug = false;

	float ambient = 0.5f;
	int sampleRadius = 2.0f;


	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> duration = end - start;
	std::cout << "-----Time taken to load objects: " << duration.count() << " seconds-----\n";


	// Main while loop
	while (!glfwWindowShouldClose(window))
	{
		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// main window
		ImGui::Begin("Another Window", &show_demo_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)

		ImGui::SliderFloat("Ambient", &ambient, 0.0f, 20.0f);
		ImGui::SliderFloat3("light pos", &lightPos[0], 0.0f, 20.0f);
		ImGui::SliderFloat3("camera position", &camera.position[0], 0.0f, 20.0f);
		ImGui::SliderFloat3("camera position", &camera.orientation[0], 0.0f, 20.0f);

		if (ImGui::Button("+"))
			sampleRadius++;
		ImGui::SameLine();
		if (ImGui::Button("-") && sampleRadius >= 0)
			sampleRadius--;
		ImGui::SameLine();
		ImGui::Text("counter = %d", sampleRadius);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

		ImGui::Text("Hello from another window!");
		ImGui::Checkbox("Debug Mode", &debug);
		ImGui::SameLine();
		ImGui::Checkbox("Show debug window", &show_debug_window);
		ImGui::SameLine();
		ImGui::Checkbox("Camera lock", &camera_control_enabled);
		ImGui::ColorEdit4("Text Color", &lightColor[0]);
		ImGui::Checkbox("Enable face culling", &face_culling_enabled);

		ImGui::End();

		if (show_style_editor)
			ImGui::ShowStyleEditor();
		if (show_debug_window) {
			ImGui::Begin("Debug Window");
			{
				// Using a Child allow to fill all the space of the window.
				// It also alows customization
				ImGui::BeginChild("Debug Window");
				// Get the size of the child (i.e. the whole draw size of the windows).
				ImVec2 wsize = ImGui::GetWindowSize();
				glActiveTexture(GL_TEXTURE0 + 2);
				glBindTexture(GL_TEXTURE_2D, shadowMap);
				ImGui::Image((ImTextureID)shadowMap, wsize, ImVec2(0, 1), ImVec2(1, 0));
				ImGui::EndChild();
			}
			ImGui::End();
		}

		// Rendering
		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		float currentTime = glfwGetTime();
		float timeDiff = currentTime - lastTime;
		frameCounter++;

		if (timeDiff >= 1 / 2) {
			std::string FPS = std::to_string((1.0 / timeDiff) * frameCounter);
			std::string ms = std::to_string((timeDiff / frameCounter) * 1000);
			std::string updatedTitle = "Graphic Engine - " + FPS + "FPS / " + ms + "ms";
			glfwSetWindowTitle(window, updatedTitle.c_str());
			lastTime = currentTime;
			frameCounter = 0;
		}

		// camera inputs
		if (camera_control_enabled) {
			camera.processInput(window);
			camera.cameraViewUpdate();
		}

		glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
		
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// render scene from light's point of view
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE0);

		glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
		glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
		glm::mat4 lightSpaceMatrix = lightSpaceMatrix = lightProjection * lightView;

		shadowMapShader.Activate();
		shadowMapShader.setMat4("lightProjection", lightSpaceMatrix);
		shadowMapShader.setMat4("matrix", planeModel);
		plane.Draw(shadowMapShader, camera);
		shadowMapShader.setMat4("matrix", objMatrix);
		shadowMapShader.setBool("hasAnimation", false);
		ourModel.Draw(shadowMapShader, camera);
		shadowMapShader.setBool("hasAnimation", true);
		shadowMapShader.setMat4("matrix", aruObjMatrix);

		auto aru_transforms_shadow = aru_animator.GetFinalBoneMatrices();
		for (int i = 0; i < aru_transforms_shadow.size(); ++i)
			shadowMapShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", aru_transforms_shadow[i]);
		aruModel.Draw(shadowMapShader, camera);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// reset viewport
		//if (face_culling_enabled) {
		//	glEnable(GL_DEPTH_TEST);
		//	glEnable(GL_CULL_FACE);
		//	glCullFace(GL_BACK);
		//	glFrontFace(GL_CCW);
		//}
		//else {
		//	glDisable(GL_CULL_FACE);
		//}

		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		debugDepthQuad.Activate();
		debugDepthQuad.setFloat("near_plane", near_plane);
		debugDepthQuad.setFloat("far_plane", far_plane);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, shadowMap);
		if (debug) renderQuad();

		glActiveTexture(GL_TEXTURE0 + 2);
		glBindTexture(GL_TEXTURE_2D, shadowMap);


		float cf = glfwGetTime();
		float dt = cf - lf;
		lf = cf;
		//animator.UpdateAnimation(dt);
		aru_animator.UpdateAnimation(dt);


		modelShader.Activate();
		glCullFace(GL_FRONT);
		modelShader.setMat4("lightProjection", lightSpaceMatrix);
		modelShader.setVec4("lightColor", lightColor);
		modelShader.setInt("sampleRadius", sampleRadius);
		modelShader.setFloat("ambientIntensity", ambient);
		aruModelShader.setBool("hasAnimation", false);
		glUniform1i(glGetUniformLocation(modelShader.ID, "shadowMap"), 2);
		glUniform3f(glGetUniformLocation(modelShader.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(glGetUniformLocation(modelShader.ID, "camPos"), camera.position.x, camera.position.y, camera.position.z);
		glCullFace(GL_BACK);
		ourModel.Draw(modelShader, camera);

		planeShader.Activate();
		glCullFace(GL_FRONT);
		planeShader.setMat4("lightProjection", lightSpaceMatrix);
		planeShader.setVec4("lightColor", lightColor);
		planeShader.setInt("sampleRadius", sampleRadius);
		planeShader.setFloat("ambientIntensity", ambient);
		aruModelShader.setBool("hasAnimation", false);
		glUniform1i(glGetUniformLocation(planeShader.ID, "shadowMap"), 2);
		glUniform3f(glGetUniformLocation(planeShader.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(glGetUniformLocation(planeShader.ID, "camPos"), camera.position.x, camera.position.y, camera.position.z);
		glCullFace(GL_BACK);
		plane.Draw(planeShader, camera);

		aruModelShader.Activate();
		aruModelShader.setMat4("lightProjection", lightSpaceMatrix);
		aruModelShader.setVec4("lightColor", lightColor);
		aruModelShader.setInt("sampleRadius", sampleRadius);
		aruModelShader.setFloat("ambientIntensity", ambient);
		aruModelShader.setBool("hasAnimation", true);
		glUniform1i(glGetUniformLocation(aruModelShader.ID, "shadowMap"), 2);
		glUniform3f(glGetUniformLocation(aruModelShader.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(glGetUniformLocation(aruModelShader.ID, "camPos"), camera.position.x, camera.position.y, camera.position.z);

		auto aru_transforms = aru_animator.GetFinalBoneMatrices();
		for (int i = 0; i < aru_transforms.size(); ++i)
			aruModelShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", aru_transforms[i]);
		aruModel.Draw(aruModelShader, camera);

		skybox.render(camera);

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		processProgramInput(window);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

void processProgramInput(GLFWwindow* window)
{
	//glfwGetKey(window, GLFW_KEY_F5) == GLFW_PRESS ? debug = true : debug = false;
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int w, int h)
{
	width = w;
	height = h;
	glViewport(0, 0, width, height);
}

void Rotate(glm::mat4 matrix, Shader& shader) {
	float currentFrame = glfwGetTime();
	float deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	float rotationSpeed = 1;
	rotationAngle += deltaTime * rotationSpeed;

	matrix = glm::rotate(matrix, rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "matrix"), 1, GL_FALSE, glm::value_ptr(matrix));

}


