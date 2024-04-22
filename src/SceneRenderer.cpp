#include "SceneRenderer.h"
#include "graphics/components/headers/ModelComponent.h"

unsigned int SceneRenderer::width = 0.0f;
unsigned int SceneRenderer::height = 0.0f;

float SceneRenderer::lastFrame = 0.0f;
float SceneRenderer::lf = 0.0f;
float SceneRenderer::lastTime = 0.0f;
unsigned int SceneRenderer::frameCounter = 0.0f;
float SceneRenderer::rotationAngle = 0.0f;
float SceneRenderer::angle = 0.0f;
float SceneRenderer::radius = 0.0f;
float SceneRenderer::angularSpeed = 0.0f;
Platform SceneRenderer::platform = PLATFORM_UNDEFINED;
const Platform SceneRenderer::supportPlatform[] = { PLATFORM_OPENGL };

float SceneRenderer::ambient = 0.5f;
int SceneRenderer::sampleRadius = 2.0f;
glm::vec4 SceneRenderer::lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
glm::vec3 SceneRenderer::lightPos = glm::vec3(0.5f, 4.5f, 5.5f);
Camera* SceneRenderer::cameraController = nullptr;
GLFWwindow* SceneRenderer::window = nullptr;

ImGuiController SceneRenderer::guiController(true);


void SceneRenderer::renderQuad()
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

void SceneRenderer::setUniform(Shader& shader, glm::mat4& matrix, bool hasAnimation, Light& light, glm::mat4& lightCastMatrix, glm::mat4& modelMatrix, Camera& camera) {
	shader.Activate();
	glCullFace(GL_FRONT);
	shader.setMat4("lightProjection", lightCastMatrix);
	shader.setVec4("lightColor", lightColor);
	shader.setInt("sampleRadius", sampleRadius);
	shader.setFloat("ambientIntensity", ambient);
	shader.setBool("hasAnimation", hasAnimation);
	shader.setInt("shadowMap", 2);
	shader.setVec3("lightPos", lightPos);
	shader.setMat4("matrix", modelMatrix);
	shader.setMat4("mvp", camera.getMVP());
	glCullFace(GL_BACK);
}

int SceneRenderer::init(Platform platform) {
	SceneRenderer::platform = platform;
	for (auto& p : supportPlatform) {
		if (platform != p) {
			throw std::runtime_error("Platform unsupported");

		}
	}

	if (SceneRenderer::platform == PLATFORM_OPENGL) {
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		// Initialize GLFW
		if (!glfwInit())
			return -1;

		// Get primary monitor
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		if (!monitor) {
			std::cerr << "Failed to get primary monitor" << std::endl;
			glfwTerminate();
			return -1;
		}

		// Get monitor video mode
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);
		if (!mode) {
			std::cerr << "Failed to get video mode" << std::endl;
			glfwTerminate();
			return -1;
		}

		width = mode->width * 2 / 3;
		height = mode->height * 2 / 3;
	}
	return 0;
}

int SceneRenderer::start(const char* title) {
	if(platform == PLATFORM_UNDEFINED)
		throw std::runtime_error("Platform not specified have you called init() with supported platform yet?");
	else if (platform == PLATFORM_OPENGL) {
		window = glfwCreateWindow(width, height, title, NULL, NULL);

		if (window == NULL)
		{
			std::cerr << "Failed to create GLFW window" << std::endl;
			glfwTerminate();
			return -1;
		}
		glfwMakeContextCurrent(window);
		gladLoadGL();
	}
	return 0;
}

int SceneRenderer::end() {
	if (platform == PLATFORM_OPENGL) {
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		glfwDestroyWindow(window);
		glfwTerminate();
	}
	return 0;
}



int SceneRenderer::renderScene() {
	glViewport(0, 0, width, height);
	glEnable(GL_DEPTH_TEST);

	Camera camera(width, height, glm::vec3(-6.5f, 3.5f, 8.5f), glm::vec3(0.5, -0.2, -1.0f));
	cameraController = &camera;

	auto start = std::chrono::high_resolution_clock::now();

	Shader modelShader("Shaders/default.vert", "Shaders/default.frag");
	glm::mat4 objMatrix = glm::mat4(1.0f);
	//ModelComponent* reimu = new ModelComponent("Models/reimu/reimu.obj");
	//glm::vec3 scale = glm::vec3(0.25f, 0.25f, 0.25f);
	//reimu->scale(scale);
	//std::cout << reimu.getID() << "\n";

	Shader aruModelShader("Shaders/default.vert", "Shaders/default.frag");
	glm::mat4 aruObjMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f, 0.0f, 0.0f));
	aruModelShader.Activate();
	modelShader.setMat4("matrix", aruObjMatrix);
	Model aruModel("Models/aru/aru.gltf");
	Animation aru_animation("Models/aru/aru.gltf", &aruModel);
	Animator aru_animator(&aru_animation);

	SkyboxComponent skybox;
	skybox.setUniform();


	PlaneComponent plane;

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

	DepthMap depthMap;

	bool show_demo_window = false;
	bool show_style_editor = false;
	bool show_debug_window = true;
	bool camera_control_enabled = true;
	bool face_culling_enabled = false;
	bool draw_frame_buffer = true;
	bool debug = false;
	bool animate_enable = true;
	bool show_navigator = true;



	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> duration = end - start;
	std::cout << "-----Time taken to load objects: " << duration.count() << " seconds-----\n";

	glm::vec3 translateVector(0.0f, 0.0f, 0.0f);
	glm::vec3 scaleVector(1.0f, 1.0f, 1.0f);

	FrameBuffer framebuffer(width, height);

	Shader frameShaderProgram("src/apps/frame-buffer/framebuffer.vert", "src/apps/frame-buffer/framebuffer.frag");
	frameShaderProgram.Activate();
	frameShaderProgram.setFloat("screenTexture", 0);


	guiController.init(window, width, height);

	float near_plane = 1.0f, far_plane = 12.5f;
	glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
	glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
	glm::mat4 lightSpaceMatrix;
	debugDepthQuad.Activate();
	debugDepthQuad.setInt("depthMap", 0);
	debugDepthQuad.setFloat("near_plane", near_plane);
	debugDepthQuad.setFloat("far_plane", far_plane);

	bool firstFrame = true;

	// Main while loop
	while (!glfwWindowShouldClose(window))
	{
		int countDrawCall = 0;

		guiController.start();

		guiController.render();

		// main window

		// Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		if(ImGui::Begin("Global Control", &show_demo_window))
		ImGui::SliderFloat("Ambient", &ambient, 0.0f, 20.0f);
		ImGui::SliderFloat3("light pos", &lightPos[0], 0.0f, 20.0f);
		if (ImGui::SliderFloat3("translate", &translateVector[0], -10.0f, 10.0f, 0)) {
			plane.translate(translateVector);
		}

		if (ImGui::SliderFloat3("scale", &scaleVector[0], -10.0f, 10.0f, 0)) {
			plane.scale(scaleVector);
		}

		if (ImGui::Button("+"))
			sampleRadius++;
		ImGui::SameLine();
		if (ImGui::Button("-") && sampleRadius >= 0)
			sampleRadius--;
		ImGui::SameLine();
		ImGui::Text("counter = %d", sampleRadius);
		ImGui::Text("Hello from another window!");
		ImGui::Checkbox("Debug Mode", &debug);
		ImGui::SameLine();
		ImGui::Checkbox("Show debug window", &show_debug_window);
		ImGui::SameLine();
		ImGui::Checkbox("Camera lock", &camera_control_enabled);
		ImGui::ColorEdit4("Text Color", &lightColor[0]);
		ImGui::Checkbox("Enable face culling", &face_culling_enabled);
		ImGui::Checkbox("Enable animation", &animate_enable);
		ImGui::Checkbox("Show navigator", &show_navigator);
		ImGui::End();


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

		camera.cameraViewUpdate();

		// render scene from light's point of view
		glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glClear(GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE0);

		lightSpaceMatrix = lightSpaceMatrix = lightProjection * lightView;
		Light light = Light(lightPos, lightColor, ambient, lightSpaceMatrix);


		shadowMapShader.Activate();
		shadowMapShader.setMat4("lightProjection", lightSpaceMatrix);

		plane.renderShadow(shadowMapShader, camera);

		//reimu.renderShadow(shadowMapShader, camera);
		OpenGLController::renderShadow(shadowMapShader, camera);
		shadowMapShader.setBool("hasAnimation", true);
		shadowMapShader.setMat4("matrix", aruObjMatrix);

		auto aru_transforms_shadow = aru_animator.GetFinalBoneMatrices();
		for (int i = 0; i < aru_transforms_shadow.size(); ++i)
			shadowMapShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", aru_transforms_shadow[i]);
		aruModel.Draw(shadowMapShader, camera);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		framebuffer.Bind();
		// reset viewport
		if (face_culling_enabled) {
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			glFrontFace(GL_CCW);
		}
		else {
			glDisable(GL_CULL_FACE);
		}

		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		debugDepthQuad.Activate();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, shadowMap);
		if (debug) renderQuad();

		glActiveTexture(GL_TEXTURE0 + 2);
		glBindTexture(GL_TEXTURE_2D, shadowMap);


		float cf = glfwGetTime();
		float dt = cf - lf;
		lf = cf;
		if (animate_enable)
			aru_animator.UpdateAnimation(dt);

		//reimu.render(camera, light);
		OpenGLController::render(camera, light);

		plane.render(camera, light);

		setUniform(aruModelShader, objMatrix, true, light, lightSpaceMatrix, aruObjMatrix, camera);
		auto aru_transforms = aru_animator.GetFinalBoneMatrices();
		for (int i = 0; i < aru_transforms.size(); ++i)
			aruModelShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", aru_transforms[i]);
		aruModel.Draw(aruModelShader, camera);

		skybox.render(camera);


		ImGui::Begin("Application Window");
		{
			// Using a Child allow to fill all the space of the window.
			ImGui::BeginChild("View window");
			// Get the size of the child (i.e. the whole draw size of the windows).
			ImVec2 wsize = ImGui::GetWindowSize();
			glBindTexture(GL_TEXTURE_2D, framebuffer.texture);
			ImGui::Image((ImTextureID)framebuffer.texture, wsize, ImVec2(0, 1), ImVec2(1, 0));

			// camera inputs
			cameraController->updateViewResize(ImGui::GetWindowWidth(), ImGui::GetWindowHeight());
			if (ImGui::IsItemHovered())
				camera.processInput(window);

			float viewManipulateRight = ImGui::GetIO().DisplaySize.x;
			float viewManipulateTop = 0;
			if (show_navigator) {
				auto v = &camera.getViewMatrix()[0][0];
				auto p = glm::value_ptr(camera.getProjectionMatrix());
				glm::mat4 transform = plane.getModelMatrix();
				ImGuizmo::SetOrthographic(false);
				ImGuizmo::SetDrawlist();
				float wd = (float)ImGui::GetWindowWidth();
				float wh = (float)ImGui::GetWindowHeight();
				glm::mat4 modelMat = plane.getModelMatrix();
				ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, wd, wh);
				glm::mat4 identity(1.0f);
				
				ImGuizmo::DrawCubes(v, p, glm::value_ptr(transform), 1);
				ImGuizmo::DrawGrid(v, p, glm::value_ptr(identity), 100.f);
				bool res = ImGuizmo::Manipulate(v, p, ImGuizmo::OPERATION::TRANSLATE, ImGuizmo::WORLD, glm::value_ptr(transform));
				viewManipulateRight = ImGui::GetWindowPos().x + wd;
				viewManipulateTop = ImGui::GetWindowPos().y;
				ImGuizmo::ViewManipulate(v, 5.0f, ImVec2(viewManipulateRight - 128, viewManipulateTop), ImVec2(128, 128), 0x10101010);
				if (ImGuizmo::IsUsing()) {
					translateVector = glm::vec3(transform[3]);
					plane.translate(translateVector);

				}
			}
			ImGui::EndChild();
		}
		ImGui::End();

		//frameShaderProgram.Activate();
		//glDisable(GL_DEPTH_TEST); // prevents framebuffer rectangle from being discarded
		//glBindTexture(GL_TEXTURE_2D, framebuffer.texture);
		//glActiveTexture(GL_TEXTURE0);
		//renderQuad();

		framebuffer.Unbind();



		ImGui::Begin("Debug Window");
		{
			std::string countVertices = "Vertices: " + std::to_string(countDrawCall * 3);
			ImGui::Text(countVertices.c_str());
			countVertices = "Triangles: " + std::to_string(countDrawCall);
			ImGui::Text(countVertices.c_str());
			// Using a Child allow to fill all the space of the window.
			// It also alows customization
			ImGui::BeginChild("Debug shadow window");
			// Get the size of the child (i.e. the whole draw size of the windows).
			ImVec2 wsize = ImGui::GetWindowSize();
			if (show_debug_window) 
				ImGui::Image((ImTextureID)shadowMap, wsize, ImVec2(0, 1), ImVec2(1, 0));
			ImGui::EndChild();
		}
		ImGui::End();


		guiController.end();

		glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
		processProgramInput(window);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	return 0;
}

void SceneRenderer::processProgramInput(GLFWwindow* window)
{
	//glfwGetKey(window, GLFW_KEY_F5) == GLFW_PRESS ? debug = true : debug = false;
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void SceneRenderer::framebuffer_size_callback(GLFWwindow* window, int w, int h)
{
	width = w;
	height = h;
	cameraController->updateViewResize(width, height);
	glViewport(0, 0, width, height);
}
