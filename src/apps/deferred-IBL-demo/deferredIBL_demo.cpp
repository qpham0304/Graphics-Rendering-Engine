#include "deferredIBL_demo.h"

int deferredIBL_demo::show_demo()
{
    int width = SceneRenderer::width;
    int height = SceneRenderer::height;
    Camera camera(width, height, glm::vec3(-6.5f, 3.5f, 8.5f), glm::vec3(0.5, -0.2, -1.0f));
    ImGuiController guiController;
    bool guiOn = true;
    if (guiOn)
        guiController.init(SceneRenderer::window, width, height);
    float frameCounter = 0.0f;
    float lastFrame = 0.0f;
    float deltaTime = 0.0f;
    unsigned int cubeVAO = 0;
    unsigned int cubeVBO = 0;

    Shader lightShader("Shaders/light.vert", "Shaders/light.frag");

    SkyboxComponent skybox;

    FrameBuffer colorPassFBO(width, height);
    FrameBuffer ssrSceneFBO(width, height);
    FrameBuffer finalSceneFBO(width, height);

    OpenGLController::addComponent("Models/backpack/backpack.obj");
    std::vector<Component*> components;
    for (const auto& pair : OpenGLController::components) {
        components.push_back(pair.second.get());
    }

    std::vector<Light> lights;
    lights.push_back(Light(glm::vec3(2.0, 0.5, 2.0), glm::vec4(1.0)));
    lights.push_back(Light(glm::vec3(-2.0, 0.5, 2.0), glm::vec4(1.0)));

    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    glEnable(GL_DEPTH_TEST);
    while (!glfwWindowShouldClose(SceneRenderer::window)) {
        camera.cameraViewUpdate();
        camera.processInput(SceneRenderer::window);

        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;

        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        frameCounter++;
        if (deltaTime >= 1 / 2) {
            std::string FPS = std::to_string((1.0 / deltaTime) * frameCounter);
            std::string ms = std::to_string((deltaTime / frameCounter) * 1000);
            std::string updatedTitle = "Deferred Shading Demo - " + FPS + "FPS / " + ms + "ms";
            glfwSetWindowTitle(SceneRenderer::window, updatedTitle.c_str());
            lastFrame = currentFrame;
            frameCounter = 0;
        }

        glfwPollEvents();
        glfwSwapBuffers(SceneRenderer::window);
    }

	return 0;
}

int deferredIBL_demo::run()
{
	return show_demo();
}
