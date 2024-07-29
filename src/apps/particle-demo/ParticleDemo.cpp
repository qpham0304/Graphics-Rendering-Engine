#include "ParticleDemo.h"
#include "ParticleGeometry.h"
#include "../../graphics/core/OpenGL/BloomRenderer.h"
#include "../../core/features/Timer.h"


int ParticleDemo::show_demo()
{
    int width = SceneRenderer::width;
    int height = SceneRenderer::height;
    GLFWwindow* window = SceneRenderer::window;
    Camera camera(width, height, glm::vec3(-3.5f, 1.5f, 5.5f), glm::vec3(0.5, -0.2, -1.0f));
    
    ImGuiController guiController;
    bool guiOn = true;
    if (guiOn)
        guiController.init(window, width, height);

    float speed = 0.001;
    bool pause = true;
    bool reset = false;
    bool isPopulating = false;
    glm::vec3 spawnArea(100.0, 10.0, 100.0);
    glm::vec3 direction(0.0, 0.0, 0.0);
    unsigned int numInstances = 100000;
    int numRender = numInstances;
    int heightLimit = 100.0;
    glm::vec3 particleSize(0.1, 0.1, 0.1);
    glm::vec2 randomRange(glm::vec2(-5.0, 5.0));

    SkyboxComponent skybox;
    FrameBuffer applicationFBO(width, height, GL_RGBA16F);
    ParticleGeometry particleRenderer;
    ParticleControl particleControl(randomRange, spawnArea, heightLimit, -heightLimit, numInstances, particleSize);
    particleRenderer.init(particleControl);

    std::vector<Component*> components;
    for (const auto& pair : OpenGLController::components) {
        components.push_back(pair.second.get());
    }

    float frameCounter = 0.0f;
    float lastFrame = 0.0f;
    float deltaTime = 0.0f;
    int firstFrame = 0;
    int frameIndex = 1;
    unsigned int cubeVAO = 0;
    unsigned int cubeVBO;

    Shader lightShader("Shaders/light.vert", "Shaders/light.frag");
    Shader particleShader("Shaders/particle.vert", "Shaders/particle.frag");
    Shader renderScene("Shaders/postProcess/renderQuad.vert", "Shaders/postProcess/renderQuad.frag");

    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    glEnable(GL_DEPTH_TEST);

    //glEnable(GL_CULL_FACE);
    //glFrontFace(GL_CCW);
    //glCullFace(GL_FRONT);
    std::vector<glm::mat4> matrixModels;
    auto lambda = [particleControl, particleSize, particleRenderer](std::vector<glm::mat4> &list, bool& isPopulating) {
        isPopulating = true;
        list.clear();
        for (int i = 0; i < particleControl.numInstances; i++) {
            list.push_back(Utils::Random::createRandomTransform(particleControl.spawnArea, particleSize));
        }
        std::mutex mtx;
        std::lock_guard<std::mutex> lock(mtx);
        isPopulating = false;
    };

    float velocity = 0.0;
    while (!glfwWindowShouldClose(window)) {
        camera.onUpdate();
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;

        frameCounter++;
        if (deltaTime >= 1 / 2) {
            std::string FPS = std::to_string((1.0 / deltaTime) * frameCounter);
            std::string ms = std::to_string((deltaTime / frameCounter) * 1000);
            std::string updatedTitle = "Deferred Shading Demo - " + FPS + "FPS / " + ms + "ms";
            glfwSetWindowTitle(window, updatedTitle.c_str());
            lastFrame = currentFrame;
            frameCounter = 0;
        }
        
        applicationFBO.Bind();
        glViewport(0.0, 0.0, width, height);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // RGBA
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //particleRenderer.render(lightShader, camera, numRender, speed, pause);
        particleRenderer.render(particleShader, camera, numRender, speed, pause);
        skybox.render(camera);

        applicationFBO.Unbind();

        if (guiOn) {
            guiController.start();

            if (ImGui::Begin("control")) {
                ImGui::BeginChild("gBuffers textures");
                ImVec2 wsize = ImGui::GetWindowSize();
                ImGui::DragFloat("Falling speed", &speed, 0.01, -10.0, 10.0);
                ImGui::DragInt("Num Instances", &numRender, particleControl.numInstances/100.0, 0, particleControl.numInstances, 0, true);
                if (ImGui::DragFloat3("Spawn Area", glm::value_ptr(particleControl.spawnArea), 0.1, 0, 1000.0, 0, true)) {
                    Timer timer;
                    particleRenderer.clear();
                    particleRenderer.init(particleControl);
                    //if (isPopulating) {
                    //    Console::println("populate");
                    //    particleRenderer.matrixModels = matrixModels;
                    //}
                }
                ImGui::Checkbox("Pause", &pause);
                ImGui::SameLine();
                if (ImGui::Button("Reset")) {
                    particleRenderer.reset();
                }
                ImGui::EndChild();
                ImGui::End();
            }

            if (ImGui::Begin("Application window")) {
                ImGui::BeginChild("Child");
                ImVec2 wsize = ImGui::GetWindowSize();
                int wWidth = static_cast<int>(ImGui::GetWindowWidth());
                int wHeight = static_cast<int>(ImGui::GetWindowHeight());
                camera.updateViewResize(wWidth, wHeight);
                ImGui::Image((ImTextureID)applicationFBO.texture, wsize, ImVec2(0, 1), ImVec2(1, 0));
                if (firstFrame <= 1) {
                    camera.processInput(window);
                    firstFrame++;
                }
                else if (ImGui::IsItemHovered()) {
                    camera.processInput(window);
                }
                ImGui::EndChild();
                ImGui::End();
            }

            guiController.end();
        }
        else {
            renderScene.Activate();
            renderScene.setInt("scene", 0);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, applicationFBO.texture);
            Utils::OpenGL::Draw::drawQuad();
            camera.processInput(window);
        }
        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    return 0;
}

int ParticleDemo::run()
{
    return show_demo();
}
