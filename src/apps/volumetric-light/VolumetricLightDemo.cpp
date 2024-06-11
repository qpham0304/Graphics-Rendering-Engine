#include "VolumetricLightDemo.h"

int VolumetricLightDemo::show_demo() {
    float width = SceneRenderer::width;
    float height = SceneRenderer::height;

    Camera camera(width, height, glm::vec3(-6.5f, 3.5f, 8.5f), glm::vec3(0.5, -0.2, -1.0f));
    SkyboxComponent skybox;

    bool guiOn = true;
    ImGuiController guiController;
    if (guiOn)
        guiController.init(SceneRenderer::window, width, height);

    Shader postProcessShader("Shaders/postProcess/renderQuad.vert", "Shaders/postProcess/renderQuad.frag");
    Shader pbrShader("src/apps/volumetric-light/shaders/pbr.vert", "src/apps/volumetric-light/shaders/pbr.frag");
    Shader lightShader("Shaders/light.vert", "Shaders/light.frag");
    Shader shadowMapShader("Shaders/shadowMap.vert", "Shaders/shadowMap.frag");

    Model reimu("Models/reimu/reimu.obj");
    Model sponza("Models/sponza/sponza.obj");

    FrameBuffer applicationFBO(width, height);
    unsigned int cubeVAO = 0;
    unsigned int cubeVBO = 0;

    DepthMap depthMap;

    glm::vec4 lightColor(1.0, 1.0, 1.0, 1.0);
    glm::vec3 lightPosition(5.0, 5.0, 5.0);
    glm::mat4 model = glm::mat4(1.0f);

    postProcessShader.Activate();
    postProcessShader.setInt("scene", 0);

    //Texture planeTex("Textures/wood.png", "diffuseMap");

    float frameCounter = 0.0f;
    float lastFrame = 0.0f;
    float deltaTime = 0.0f;

    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    glEnable(GL_DEPTH_TEST);
    //glDepthFunc(GL_LEQUAL);
    while (!glfwWindowShouldClose(SceneRenderer::window)) {
        camera.cameraViewUpdate();
        camera.processInput(SceneRenderer::window);

        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;

        frameCounter++;
        if (deltaTime >= 1 / 2) {
            std::string FPS = std::to_string((1.0 / deltaTime) * frameCounter);
            std::string ms = std::to_string((deltaTime / frameCounter) * 1000);
            std::string updatedTitle = "Deferred Shading Demo - " + FPS + "FPS / " + ms + "ms";
            glfwSetWindowTitle(SceneRenderer::window, updatedTitle.c_str());
            lastFrame = currentFrame;
            frameCounter = 0;
        }

        Light light(lightPosition, lightColor);
        glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 25.0f);
        glm::mat4 lightView = glm::lookAt(light.position, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
        glm::mat4 lightMVP = lightProjection * lightView;
        light.mvp = lightMVP;

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // RGBA
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        depthMap.Bind();
        glViewport(0, 0, depthMap.SHADOW_WIDTH, depthMap.SHADOW_HEIGHT);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // RGBA
        glClear(GL_DEPTH_BUFFER_BIT);
        shadowMapShader.Activate();
        model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(0.10));
        shadowMapShader.setMat4("mvp", light.mvp);
        shadowMapShader.setMat4("matrix", model);
        shadowMapShader.setBool("hasAnimation", false);
        reimu.Draw(shadowMapShader);

        model = glm::scale(glm::mat4(1.0f), glm::vec3(0.2f));
        pbrShader.setMat4("matrix", model);
        sponza.Draw(pbrShader);
        depthMap.Unbind();

        applicationFBO.Bind();
        glViewport(0, 0, width, height);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // RGBA
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        lightShader.Activate();
        model = glm::mat4(1.0);
        lightShader.setMat4("mvp", camera.getMVP());
        model = glm::translate(model, light.position);
        model = glm::scale(model, glm::vec3(0.125f));
        lightShader.setMat4("matrix", model);
        lightShader.setVec3("lightColor", light.color);
        Utils::Draw::drawSphere(cubeVAO, cubeVBO);
        
        pbrShader.Activate();
        model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(0.10));
        glm::mat3 normalMatrix = model;
        pbrShader.setMat4("matrix", model);
        pbrShader.setMat4("mvp", camera.getMVP());
        pbrShader.setBool("hasAnimation", false);
        pbrShader.setMat3("normalMatrix", glm::transpose(glm::inverse(normalMatrix)));
        pbrShader.setVec3("lightPos", light.position);
        pbrShader.setVec3("camPos", camera.getPosition());
        pbrShader.setMat4("lightMVP", light.mvp);
        pbrShader.setInt("depthMap", 10);

        glActiveTexture(GL_TEXTURE10);
        glBindTexture(GL_TEXTURE_2D, depthMap.texture);
        reimu.Draw(pbrShader);

        model = glm::scale(glm::mat4(1.0f), glm::vec3(0.2f));
        pbrShader.setMat4("matrix", model);
        //Utils::Draw::drawQuad();
        sponza.Draw(pbrShader);

        //skybox.render(camera);
        applicationFBO.Unbind();

        if (guiOn) {
            guiController.start();
            if (ImGui::Begin("control")) {
                ImGui::BeginChild("gBuffers textures");
                ImVec2 wsize = ImGui::GetWindowSize();
                ImGui::Image((ImTextureID)depthMap.texture, wsize, ImVec2(0, 1), ImVec2(1, 0));

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
                if (ImGui::IsItemHovered())
                    camera.processInput(SceneRenderer::window);
                ImGui::EndChild();
                ImGui::End();
            }
            guiController.end();
        }
        else {
            postProcessShader.Activate();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, applicationFBO.texture);
            Utils::Draw::drawQuad();
        }

        glfwPollEvents();
        glfwSwapBuffers(SceneRenderer::window);
    }
    return 0;
}

int VolumetricLightDemo::run() {
    try {
        show_demo();
        return 0;
    }
    catch (const std::runtime_error& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return -1;
    }
}