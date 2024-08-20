#include "deferredIBL_demo.h"
#include "ParticleGeometry.h"
#include "../../core/scene/SceneManager.h"
#include "../../core/components/MComponent.h"
#include "../../core/components/CameraComponent.h"
#include "../../core/components/CubeMapComponent.h"

static glm::vec3 lightPositions[] = {
    glm::vec3(10.0f,  1.0f, 10.0f),
    glm::vec3(10.0f,  1.0f, -10.0f),
    glm::vec3(-10.0f, 5.0f, 10.0f),
    glm::vec3(-10.0f, 5.0f, -10.0f),
};
static glm::vec4 lightColors[] = {
    //glm::vec4(900.0f, 500.0f, 500.0f, 1.0f),
    //glm::vec4(300.0f, 500.0f, 300.0f, 1.0f),
    //glm::vec4(900.0f, 400.0f, 300.0f, 1.0f),
    //glm::vec4(500.0f, 300.0f, 500.0f, 1.0f)
    glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),
    glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),
    glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),
    glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
};

void DeferredIBLDemo::setupBuffers()
{
    int width = AppWindow::width;
    int height = AppWindow::height;

    glGenFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

    glGenTextures(1, &gAlbedo);
    glBindTexture(GL_TEXTURE_2D, gAlbedo);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gAlbedo, 0);

    glGenTextures(1, &gNormal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

    glGenTextures(1, &gMetalRoughness);
    glBindTexture(GL_TEXTURE_2D, gMetalRoughness);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gMetalRoughness, 0);

    glGenTextures(1, &gEmissive);
    glBindTexture(GL_TEXTURE_2D, gEmissive);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gEmissive, 0);

    glGenTextures(1, &gDUV);
    glBindTexture(GL_TEXTURE_2D, gDUV);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, gDUV, 0);

    glGenTextures(1, &gDepth);
    glBindTexture(GL_TEXTURE_2D, gDepth);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH32F_STENCIL8, width, height, 0, 
        GL_DEPTH_STENCIL, GL_FLOAT_32_UNSIGNED_INT_24_8_REV, 0
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, gDepth, 0);

    GLenum drawBuffers1[5] = { 
        GL_COLOR_ATTACHMENT0, 
        GL_COLOR_ATTACHMENT1, 
        GL_COLOR_ATTACHMENT2, 
        GL_COLOR_ATTACHMENT3,
        GL_COLOR_ATTACHMENT4
    };
    glDrawBuffers(4, drawBuffers1);
    
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << std::hex << glCheckFramebufferStatus(GL_FRAMEBUFFER) << std::endl;
    }
}

void DeferredIBLDemo::renderColorPass()
{
    Scene& scene = *SceneManager::getInstance().getActiveScene();
    
    scene.addShader("colorPassShader", "Shaders/deferredIBL/colorPass.vert", "Shaders/deferredIBL/colorPass.frag");
    scene.getShader("colorPassShader")->Activate();
    scene.getShader("colorPassShader")->setInt("gDepth", 0);
    scene.getShader("colorPassShader")->setInt("gNormal", 1);
    scene.getShader("colorPassShader")->setInt("gAlbedo", 2);
    scene.getShader("colorPassShader")->setInt("gMetalRoughness", 3);
    scene.getShader("colorPassShader")->setInt("gEmissive", 4);
    scene.getShader("colorPassShader")->setInt("gDUV", 5);
    scene.getShader("colorPassShader")->setInt("irradianceMap", 6);
    scene.getShader("colorPassShader")->setInt("prefilterMap", 7);
    scene.getShader("colorPassShader")->setInt("brdfLUT", 8);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gDepth);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, gAlbedo);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, gMetalRoughness);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, gEmissive);
    //glActiveTexture(GL_TEXTURE5);
    //glBindTexture(GL_TEXTURE_2D, gDUV);
    
    auto list = scene.getEntitiesWith<CubeMapComponent>();
    CubeMapComponent* cubeMap = nullptr;
    if (!list.empty() && list[0].hasComponent<CubeMapComponent>()) {
        cubeMap = &list[0].getComponent<CubeMapComponent>();
        cubeMap->bindIBL(); // slot 6, 7, 8
    }

    //scene.getShader("colorPassShader")->setMat4("view", camera.getViewMatrix());
    //scene.getShader("colorPassShader")->setMat4("projection", camera.getProjectionMatrix());
    scene.getShader("colorPassShader")->setMat4("invProjection", glm::inverse(camera.getProjectionMatrix()));
    scene.getShader("colorPassShader")->setMat4("inverseView", glm::inverse(camera.getViewMatrix()));
    scene.getShader("colorPassShader")->setBool("gamma", true);
    scene.getShader("colorPassShader")->setVec3("camPos", camera.getPosition());
    //scene.getShader("colorPassShader")->setInt("width", AppWindow::width);
    //scene.getShader("colorPassShader")->setInt("height", AppWindow::height);

    for (int i = 0; i < 4; i++) {
        scene.getShader("colorPassShader")->setVec3("lights[" + std::to_string(i) + "].color", lightPositions[i]);
        scene.getShader("colorPassShader")->setVec3("lights[" + std::to_string(i) + "].position", lightColors[i]);
    }

    lightPassFBO.Bind();
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    Utils::OpenGL::Draw::drawQuad();
    lightPassFBO.Unbind();

}

void DeferredIBLDemo::renderPrePass()
{
    Scene& scene = *SceneManager::getInstance().getActiveScene();

    scene.addShader("gPassShader", "Shaders/deferredIBL/gPass.vert", "Shaders/deferredIBL/gPass.frag");
    scene.getShader("gPassShader")->Activate();
    scene.getShader("gPassShader")->setInt("albedoMap", 0);
    scene.getShader("gPassShader")->setInt("normalMap", 1);
    scene.getShader("gPassShader")->setInt("metallicMap", 2);
    scene.getShader("gPassShader")->setInt("roughnessMap", 3);
    scene.getShader("gPassShader")->setInt("aoMap", 4);
    scene.getShader("gPassShader")->setInt("emissiveMap", 5);
    scene.getShader("gPassShader")->setInt("duvMap", 6);

    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
    glClearColor(1.0f, 0.5f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    scene.getShader("gPassShader")->Activate();
    glm::mat4 model = glm::mat4(1.0);
    model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
    model = glm::scale(model, glm::vec3(30.0f));
    scene.getShader("gPassShader")->setInt("albedoMap", 0);
    scene.getShader("gPassShader")->setInt("metallicMap", 1.0);
    
    //glActiveTexture(GL_TEXTURE6);
    //glBindTexture(GL_TEXTURE_2D, duvMap);

    std::vector<Entity> entities = scene.getEntitiesWith<ModelComponent, TransformComponent>();

    for (auto& entity : entities) {
        ModelComponent model = entity.getComponent<ModelComponent>();
        glm::mat4& modelMatrix = entity.getComponent<TransformComponent>().getModelMatrix();
        glm::mat4 modelViewNormal = glm::transpose(glm::inverse(camera.getViewMatrix() * modelMatrix));
        
        scene.getShader("gPassShader")->setFloat("reflectiveMap", 1.0);
        scene.getShader("gPassShader")->setBool("invertedNormals", false);
        scene.getShader("gPassShader")->setBool("invertedTexCoords", false);
        scene.getShader("gPassShader")->setMat4("model", modelMatrix);
        scene.getShader("gPassShader")->setMat4("modelViewNormal", modelViewNormal);
        scene.getShader("gPassShader")->setMat4("mvp", camera.getMVP() * modelMatrix);
        scene.getShader("gPassShader")->setMat4("inverseViewMat", camera.getInViewMatrix());

        bool hasAnimation = entity.hasComponent<AnimationComponent>();
        scene.getShader("gPassShader")->setBool("hasAnimation", hasAnimation);

        if (hasAnimation) {
            AnimationComponent& animationComponent = entity.getComponent<AnimationComponent>();
            auto transformBoneMatricies = animationComponent.animator.GetFinalBoneMatrices();
            for (int i = 0; i < transformBoneMatricies.size(); ++i) {
                scene.getShader("gPassShader")->setMat4(
                    "finalBonesMatrices[" + std::to_string(i) + "]",
                    transformBoneMatricies[i]
                );
            }
        }

        model.model.lock()->Draw(*scene.getShader("gPassShader"));
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

DeferredIBLDemo::DeferredIBLDemo(const std::string& name) : AppLayer(name)
{
    //particleRenderer.init(particleControl);
    pbrShader.Init("Shaders/default-2.vert", "Shaders/default-2.frag");
    particleShader.Init("Shaders/particle.vert", "Shaders/particle.frag");
    lightPassFBO.Init(AppWindow::width, AppWindow::height, GL_RGBA32F, GL_RGBA, GL_FLOAT, nullptr);
}

void DeferredIBLDemo::OnAttach()
{
    AppLayer::OnAttach();
    setupBuffers();
    SceneManager::cameraController = &camera;
    LayerManager::addFrameBuffer("DeferredIBLDemo", applicationFBO);
    Scene& scene = *SceneManager::getInstance().getActiveScene();

    TransformComponent* transform;
    ModelLoadEvent event;
    AnimationLoadEvent animationLoadEvent;

    try {
        uint32_t helmetID = scene.addEntity("helmet");
        event = ModelLoadEvent("Models/DamagedHelmet/gltf/DamagedHelmet.gltf", scene.entities[helmetID]);
        EventManager::getInstance().Publish(event);
        transform = &scene.entities[helmetID].getComponent<TransformComponent>();
        transform->translate(glm::vec3(2.0, 3.0, -3.0));

        uint32_t aruID = scene.addEntity("aru");
        event = ModelLoadEvent("Models/aru/aru.gltf", scene.entities[aruID]);
        EventManager::getInstance().Publish(event);
        animationLoadEvent = AnimationLoadEvent("Models/aru/aru.gltf", scene.entities[aruID]);
        EventManager::getInstance().Publish(animationLoadEvent);
        transform = &scene.entities[aruID].getComponent<TransformComponent>();
        transform->translate(glm::vec3(-5.8, 3.0, 5.8));

        //uint32_t terrainID = scene.addEntity("terrain");
        //event = ModelLoadEvent("Models/death-valley-terrain/scene.gltf", scene.entities[terrainID]);
        //EventManager::getInstance().Publish(event);
        //transform = &scene.entities[terrainID].getComponent<TransformComponent>();
        //transform->translate(glm::vec3(0.0, -5.0, 0.0));
        //transform->rotate(glm::radians(glm::vec3(180.0, 0.0, 0.0)));
        //transform->scale(glm::vec3(50.0));

        //uint32_t cameraEntityID = scene.addEntity("camera");
        //Entity& cameraEntity = scene.entities[cameraEntityID];
        //TransformComponent& cameraTransform = cameraEntity.getComponent<TransformComponent>();
        //cameraTransform.translate(glm::vec3(-6.5f, 3.5f, 8.5f));
        //cameraEntity.addComponent<CameraComponent>(
        //    AppWindow::width,
        //    AppWindow::height,
        //    cameraTransform.translateVec,
        //    glm::vec3(0.5, -0.2, -1.0f)
        //);
        //auto pos = cameraEntity.getComponent<CameraComponent>().camera.getPosition();
        ////Console::println(pos.x, " ", pos.y, " ", pos.z);
        //cameraEntity.onCameraComponentAdded();
        //cameraTransform.translate(glm::vec3(-6.5f, 3.5f, 8.5f));

        for (int i = 0; i < 4; i++) {
            uint32_t lightID = scene.addEntity("light " + std::to_string(i));
            Entity lightEntity = scene.getEntity(lightID);
            auto& lightComponent = lightEntity.addComponent<MLightComponent>();
            lightComponent.color = lightColors[i];
            lightComponent.position = lightEntity.getComponent<TransformComponent>().translateVec;
            lightEntity.getComponent<TransformComponent>().translate(lightPositions[i]);
        }

        uint32_t cubemapID = scene.addEntity("cubemap");
        Entity cubemapEntity = scene.getEntity(cubemapID);
        cubemapEntity.addComponent<CubeMapComponent>("Textures/hdr/industrial_sunset_02_puresky_1k.hdr");
    }
    catch (std::runtime_error e) {
        Console::error(e.what());
    }

    pbrShader.Activate();
    pbrShader.setInt("albedoMap", 0);
    pbrShader.setInt("normalMap", 1);
    pbrShader.setInt("metallicMap", 2);
    pbrShader.setInt("roughnessMap", 3);
    pbrShader.setInt("aoMap", 4);
    pbrShader.setInt("emissiveMap", 5);
    pbrShader.setInt("irradianceMap", 6);
    pbrShader.setInt("prefilterMap", 7);
    pbrShader.setInt("brdfLUT", 8);

    scene.addShader("lightShader", "Shaders/light.vert", "Shaders/light.frag");
    scene.getShader("lightShader")->Activate();
    scene.getShader("lightShader")->setInt("irradianceMap", 6);

}

void DeferredIBLDemo::OnDetach()
{
    AppLayer::OnDetach();
}

void DeferredIBLDemo::OnUpdate()
{
    AppLayer::OnUpdate();
    Scene& scene = *SceneManager::getInstance().getActiveScene();

    applicationFBO.Bind();
    glViewport(0, 0, AppWindow::width, AppWindow::height);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CW);
    glCullFace(GL_FRONT);

    pbrShader.Activate();
    pbrShader.setMat4("matrix", glm::mat4(1.0f));
    pbrShader.setMat4("mvp", camera.getMVP());
    pbrShader.setVec3("camPos", camera.getPosition());
    pbrShader.setBool("gamma", true);

    auto list = scene.getEntitiesWith<CubeMapComponent>();
    CubeMapComponent* cubeMap = nullptr;
    if (!list.empty() && list[0].hasComponent<CubeMapComponent>()) {
        cubeMap = &list[0].getComponent<CubeMapComponent>();
        cubeMap->bindIBL();
    }

    auto models = scene.getEnttEntities<ModelComponent, TransformComponent>();

    for (auto entity : models) {
        ModelComponent& modelComponent = models.get<ModelComponent>(entity);
        TransformComponent& transform = models.get<TransformComponent>(entity);
        const glm::mat4& normalMatrix = transform.getModelMatrix();
        std::shared_ptr<Model> model = modelComponent.model.lock();

        if (scene.hasComponent<CameraComponent>(entity) || scene.hasComponent<AnimationComponent>(entity)) {
            continue;
        }

        if (model != nullptr) {
            pbrShader.setBool("hasAnimation", false);
            pbrShader.setBool("hasEmission", true);
            pbrShader.setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(normalMatrix))));
            pbrShader.setMat4("matrix", normalMatrix);
            model->Draw(pbrShader);
        }

        else {
            modelComponent.reset();
        }
    }

    auto animatedModels = scene.getEntitiesWith<AnimationComponent>();
    for (auto& entity : animatedModels) {
        ModelComponent& modelComponent = entity.getComponent<ModelComponent>();
        TransformComponent& transform = entity.getComponent<TransformComponent>();
        const glm::mat4& normalMatrix = transform.getModelMatrix();
        std::shared_ptr<Model> model = modelComponent.model.lock();

        pbrShader.setBool("hasAnimation", true);
        pbrShader.setBool("hasEmission", false);
        pbrShader.setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(normalMatrix))));
        pbrShader.setMat4("matrix", normalMatrix);
        
        AnimationComponent& animationComponent = entity.getComponent<AnimationComponent>();
        auto transformBoneMatricies = animationComponent.animator.GetFinalBoneMatrices();

        for (int i = 0; i < transformBoneMatricies.size(); ++i) {
            pbrShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transformBoneMatricies[i]);
        }
        model->Draw(pbrShader);

        float currentTime = static_cast<float>(glfwGetTime());
        float dt = currentTime - lf;
        lf = currentTime;
        animationComponent.animator.UpdateAnimation(dt);
    }

    std::vector<Entity> lights = scene.getEntitiesWith<MLightComponent, TransformComponent>();
    int index = 0;
    for (auto& entity : lights) {
        if (entity.hasComponent<CameraComponent>()) {
            continue;
        }

        //BUG: old light stays after removed until new one is added
        //index increment won't work for light added but not removed
        MLightComponent& light = entity.getComponent<MLightComponent>();
        TransformComponent& transform = entity.getComponent<TransformComponent>();
        pbrShader.Activate();
        // hack to simulate light size, only work with uniform scale
        glm::vec3 lightIntensity = light.color * (transform.scaleVec * transform.scaleVec);
        pbrShader.setVec3("lightColors[" + std::to_string(index) + "]", lightIntensity);
        pbrShader.setVec3("lightPositions[" + std::to_string(index) + "]", light.position);

        glm::mat4& model = transform.getModelMatrix();
        light.position = transform.translateVec;
        if (scene.getShader("lightShader")) {
            scene.getShader("lightShader")->Activate();
            scene.getShader("lightShader")->setMat4("matrix", model);
            scene.getShader("lightShader")->setVec3("lightColor", glm::normalize(light.color));
            scene.getShader("lightShader")->setMat4("mvp", camera.getMVP());
        }
        Utils::OpenGL::Draw::drawSphere(sphereVAO, indexCount);
        index++;
    }

    //particleRenderer.render(particleShader, *SceneManager::cameraController, numRender, speed, pause);

    if (cubeMap) {
        cubeMap->render(camera);
    }

    applicationFBO.Unbind();
}

void DeferredIBLDemo::OnGuiUpdate()
{
    AppLayer::OnGuiUpdate();

    Scene& scene = *SceneManager::getInstance().getActiveScene();

    if (ImGui::Begin("control")) {
        ImVec2 wsize = ImGui::GetWindowSize();
        int wWidth = static_cast<int>(ImGui::GetWindowWidth());
        int wHeight = static_cast<int>(ImGui::GetWindowHeight());

        ImGui::DragFloat("Falling speed", &speed, 0.01, -10.0, 10.0);
        ImGui::DragInt("Num Instances", &numRender, particleControl.numInstances / 100.0, 0, particleControl.numInstances, 0, true);
        if (ImGui::DragFloat3("Spawn Area", glm::value_ptr(particleControl.spawnArea), 0.1, 0, 1000.0, 0, true)) {
            particleRenderer.clear();
            particleRenderer.init(particleControl);
        }
        ImGui::Checkbox("Pause", &pause);
        ImGui::SameLine();
        if (ImGui::Button("Reset")) {
            particleRenderer.reset();
        }


        renderPrePass();
        ImGui::Text("gAlbedo");
        ImGui::Image((ImTextureID)gAlbedo, wsize, ImVec2(0, 1), ImVec2(1, 0));
        ImGui::Text("gNormal");
        ImGui::Image((ImTextureID)gNormal, wsize, ImVec2(0, 1), ImVec2(1, 0));
        ImGui::Text("gMetalRoughness");
        ImGui::Image((ImTextureID)gMetalRoughness, wsize, ImVec2(0, 1), ImVec2(1, 0));
        ImGui::Text("gEmissive");
        ImGui::Image((ImTextureID)gEmissive, wsize, ImVec2(0, 1), ImVec2(1, 0));
        ImGui::Text("gDUV");
        ImGui::Image((ImTextureID)gDUV, wsize, ImVec2(0, 1), ImVec2(1, 0));
        ImGui::Text("gDepth");
        ImGui::Image((ImTextureID)gDepth, wsize, ImVec2(0, 1), ImVec2(1, 0));

        renderColorPass();
        ImGui::Text("Color pass");
        ImGui::Image((ImTextureID)lightPassFBO.texture, wsize, ImVec2(0, 1), ImVec2(1, 0));
        ImGui::End();
    }
}

void DeferredIBLDemo::OnEvent(Event& event)
{
    AppLayer::OnEvent(event);
}

int DeferredIBLDemo::show_demo()
{
/*
    int width = AppWindow::width;
    int height = AppWindow::height;
    Camera camera(width, height, glm::vec3(-6.5f, 3.5f, 8.5f), glm::vec3(0.5, -0.2, -1.0f));
    ImGuiController guiController;
    bool guiOn = true;
    if (guiOn)
        guiController.init(AppWindow::window, width, height);

    float frameCounter = 0.0f;
    float lastFrame = 0.0f;
    float deltaTime = 0.0f;
    unsigned int cubeVAO = 0;
    unsigned int cubeVBO = 0;
    unsigned int sphereVAO = 0;
    unsigned int indexCount;

    Shader lightShader("Shaders/light.vert", "Shaders/light.frag");

    SkyboxRenderer skybox;

    FrameBuffer colorPassFBO(width, height);
    FrameBuffer ssrSceneFBO(width, height);
    FrameBuffer finalSceneFBO(width, height);
    FrameBuffer transmittanceLUT(width, height);
    FrameBuffer multipleScatteredLUT(width, height);
    FrameBuffer skyViewLUT(width, height);
    FrameBuffer atmosphereScene(width, height);

    float speed = 0.001;
    bool pause = true;
    bool reset = false;
    glm::vec3 spawnArea(100.0, 30.0, 100.0);
    glm::vec3 direction(0.0, 0.0, 0.0);
    unsigned int numInstances = 100000;
    int numRender = numInstances;
    int heightLimit = 100.0;
    glm::vec2 randomRange(glm::vec2(1.0, 5.0));
    glm::vec3 particleSize(0.1, 0.1, 0.1);
    ParticleGeometry particleRenderer;
    ParticleControl particleControl(randomRange, spawnArea, heightLimit, -heightLimit, numInstances, particleSize);
    particleRenderer.init(particleControl);

    Component helmetModel("Models/DamagedHelmet/gltf/DamagedHelmet.gltf");
    //Component terrain("Models/mountain_asset_canadian_rockies_modular/scene.gltf");
    Component terrain("Models/death-valley-terrain/scene.gltf");

    SceneManager::addComponent(helmetModel);
    SceneManager::addComponent(terrain);
    std::vector<Component*> components;
    for (const auto& pair : SceneManager::components) {
        components.push_back(pair.second.get());
    }

    std::vector<Light> lights;
    lights.push_back(Light(glm::vec3(2.0, 0.5, -30.0), glm::vec4(1000.0f, 1000.0f, 3000.0f, 1.0f)));
    lights.push_back(Light(glm::vec3(-2.0, 0.5, 2.0), glm::vec4(200.0f, 100.0f, 100.0f, 1.0f)));

    Shader pbrShader("Shaders/default-2.vert", "Shaders/default-2.frag");
    Shader equirectangularToCubemapShader("Shaders/cubemap-hdr.vert", "Shaders/equireRectToCubemap.frag");
    Shader irradianceShader("Shaders/cubemap-hdr.vert", "Shaders/irradianceConvolution.frag");
    Shader backgroundShader("Shaders/background.vert", "Shaders/background.frag");
    Shader prefilterShader("Shaders/cubemap-hdr.vert", "Shaders/prefilter.frag");
    Shader brdfShader("Shaders/brdf.vert", "Shaders/brdf.frag");

    pbrShader.Activate();
    pbrShader.setInt("albedoMap", 0);
    pbrShader.setInt("normalMap", 1);
    pbrShader.setInt("metallicMap", 2);
    pbrShader.setInt("roughnessMap", 3);
    pbrShader.setInt("aoMap", 4);
    pbrShader.setInt("emissiveMap", 5);
    pbrShader.setInt("irradianceMap", 6);
    pbrShader.setInt("prefilterMap", 7);
    pbrShader.setInt("brdfLUT", 8);

    backgroundShader.Activate();
    backgroundShader.setInt("environmentMap", 0);

    unsigned int captureFBO, captureRBO;
    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

    //---------------envcubemap set up---------------//
    unsigned int envCubemapTexture;
    glGenTextures(1, &envCubemapTexture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemapTexture);
    for (unsigned int i = 0; i < 6; ++i)
    {
        // note that we store each face with 16 bit floating point values
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 captureViews[] =
    {
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };

    unsigned int hdrTexture;
    std::string texRes = Utils::OpenGL::loadHDRTexture("Textures/hdr/industrial_sunset_02_puresky_1k.hdr", hdrTexture);
    std::cout << texRes << std::endl;

    // convert HDR equirectangular environment map to cubemap equivalent
    equirectangularToCubemapShader.Activate();
    equirectangularToCubemapShader.setInt("equirectangularMap", 0);
    equirectangularToCubemapShader.setMat4("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdrTexture);

    glViewport(0, 0, 512, 512); // don't forget to configure the viewport to the capture dimensions.
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

    for (unsigned int i = 0; i < 6; ++i)
    {
        equirectangularToCubemapShader.setMat4("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemapTexture, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Utils::OpenGL::Draw::drawCube(cubeVAO, cubeVBO); // renders a 1x1 cube
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //------------------------------------------------------------//

    // then let OpenGL generate mipmaps from first mip face (combatting visible dots artifact)
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemapTexture);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    //---------------irradiance map set up---------------//
    unsigned int irradianceMap;
    glGenTextures(1, &irradianceMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0,
            GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);
    irradianceShader.Activate();
    irradianceShader.setInt("environmentMap", 0);
    irradianceShader.setMat4("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemapTexture);

    glViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    for (unsigned int i = 0; i < 6; ++i)
    {
        irradianceShader.setMat4("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Utils::OpenGL::Draw::drawCube(cubeVAO, cubeVBO);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //------------------------------------------------------------//


    //-----------------prefilter map---------------------//
    unsigned int prefilterMap;
    glGenTextures(1, &prefilterMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // be sure to set minification filter to mip_linear 
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // generate mipmaps for the cubemap so OpenGL automatically allocates the required memory.
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    prefilterShader.Activate();
    prefilterShader.setInt("environmentMap", 0);
    prefilterShader.setMat4("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemapTexture);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    unsigned int maxMipLevels = 5;
    for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
    {
        // reisze framebuffer according to mip-level size.
        unsigned int mipWidth = static_cast<unsigned int>(128 * std::pow(0.5, mip));
        unsigned int mipHeight = static_cast<unsigned int>(128 * std::pow(0.5, mip));
        glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
        glViewport(0, 0, mipWidth, mipHeight);

        float roughness = (float)mip / (float)(maxMipLevels - 1);
        prefilterShader.setFloat("roughness", roughness);
        for (unsigned int i = 0; i < 6; ++i)
        {
            prefilterShader.setMat4("view", captureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            Utils::OpenGL::Draw::drawCube(cubeVAO, cubeVBO);
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //-----------------------------------------------------------//


    //-----------------------TLU map-----------------------------//
    unsigned int brdfLUTTexture;
    glGenTextures(1, &brdfLUTTexture);

    // pre-allocate enough memory for the LUT texture.
    glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
    // be sure to set wrapping mode to GL_CLAMP_TO_EDGE
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // then re-configure capture framebuffer object and render screen-space quad with BRDF shader.
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTexture, 0);

    glViewport(0, 0, 512, 512);
    brdfShader.Activate();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    Utils::OpenGL::Draw::drawQuad();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //-----------------------------------------------------------//

    glm::mat4 projection = glm::perspective(glm::radians(camera.getFOV()), (float)AppWindow::width / AppWindow::height, 0.1f, 100.0f);
    backgroundShader.Activate();
    backgroundShader.setMat4("projection", projection);

    ////////////////////////////////////////////////////////////
    GLuint aerialPerspectiveTexture;
    glGenTextures(1, &aerialPerspectiveTexture);
    glBindTexture(GL_TEXTURE_3D, aerialPerspectiveTexture);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA16F, 32, 32, 32, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    transmittanceLUT.Bind();
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    Shader transmittanceShader("Shaders/atmospheric.vert", "Shaders/skyAtmosphere/transmittanceLUT.frag");
    transmittanceShader.Activate();
    Utils::OpenGL::Draw::drawQuad();
    transmittanceLUT.Unbind();

    multipleScatteredLUT.Bind();
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    Shader multipleScatteredShader("Shaders/atmospheric.vert", "Shaders/skyAtmosphere/multipleScatteredLUT.frag");
    multipleScatteredShader.Activate();
    multipleScatteredShader.setInt("iChannel0", 0);
    multipleScatteredShader.setVec2("iChannelResolution", glm::vec2(width, height));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, transmittanceLUT.texture);
    Utils::OpenGL::Draw::drawQuad();
    multipleScatteredLUT.Unbind();

    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    while (!glfwWindowShouldClose(AppWindow::window)) {
        camera.onUpdate();

        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;

        frameCounter++;
        if (deltaTime >= 1 / 2) {
            std::string FPS = std::to_string((1.0 / deltaTime) * frameCounter);
            std::string ms = std::to_string((deltaTime / frameCounter) * 1000);
            std::string updatedTitle = "Deferred Shading Demo - " + FPS + "FPS / " + ms + "ms";
            glfwSetWindowTitle(AppWindow::window, updatedTitle.c_str());
            lastFrame = currentFrame;
            frameCounter = 0;
        }

        colorPassFBO.Bind();
        glViewport(0, 0, AppWindow::width, AppWindow::height);
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        pbrShader.Activate();
        pbrShader.setMat4("matrix", glm::mat4(1.0f));
        pbrShader.setMat4("mvp", camera.getMVP());
        pbrShader.setVec3("camPos", camera.getPosition());
        pbrShader.setBool("gamma", true);

        // bind pre-computed IBL data
        glActiveTexture(GL_TEXTURE0 + 6);
        glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
        glActiveTexture(GL_TEXTURE0 + 7);
        glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
        glActiveTexture(GL_TEXTURE0 + 8);
        glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);

        pbrShader.setMat4("matrix", components[0]->getModelMatrix());
        pbrShader.setBool("hasAnimation", false);
        pbrShader.setBool("hasEmission", true);
        pbrShader.setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(components[0]->getModelMatrix()))));
        components[0]->model_ptr->Draw(pbrShader);

        pbrShader.setBool("hasAnimation", false);
        pbrShader.setBool("hasEmission", false);
        components[1]->translate(glm::vec3(0.0f, -10.0f, -15.0f));
        components[1]->scale(glm::vec3(10.0f));
        glm::mat4 terrainModelMatrix = glm::rotate(components[1]->getModelMatrix(), glm::radians(180.0f), glm::vec3(1.0, 0.0, 0.0));
        pbrShader.setMat4("matrix", terrainModelMatrix);
        pbrShader.setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(terrainModelMatrix))));
        components[1]->model_ptr->Draw(pbrShader);

        for (unsigned int i = 0; i < lights.size(); ++i)
        {
            pbrShader.setVec3("lightPositions[" + std::to_string(i) + "]", lights[i].position);
            pbrShader.setVec3("lightColors[" + std::to_string(i) + "]", lights[i].color);
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, lights[i].position);
            model = glm::scale(model, glm::vec3(0.5f));
            pbrShader.setMat4("matrix", model);
            pbrShader.setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
            Utils::OpenGL::Draw::drawSphere(sphereVAO, indexCount);
        }
        particleRenderer.render(lightShader, camera, numRender, speed, pause);

        skybox.updateTexture(envCubemapTexture);
        skybox.render(camera);

        colorPassFBO.Unbind();

        //finalSceneFBO.Bind();
        //glViewport(0, 0, AppWindow::width, AppWindow::height);
        //glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        //Shader atmosphericShader("Shaders/atmospheric.vert", "Shaders/atmospheric.frag");
        //atmosphericShader.Activate();
        //atmosphericShader.setInt("colorScene", 0);
        //atmosphericShader.setVec2("iResolution", glm::vec2(width, height));
        //atmosphericShader.setFloat("iTime", currentFrame);
        //atmosphericShader.setVec2("iMouse", glm::vec2(0.0, 0.0));
        //atmosphericShader.setVec3("camPos", camera.getPosition());
        //glActiveTexture(GL_TEXTURE0);
        //glBindTexture(GL_TEXTURE_2D, colorPassFBO.texture);
        //Utils::OpenGL::Draw::drawQuad();
        //finalSceneFBO.Unbind();

        //////////////////////////////////


        skyViewLUT.Bind();
        glViewport(0, 0, AppWindow::width, AppWindow::height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        Shader skyViewShader("Shaders/atmospheric.vert", "Shaders/skyAtmosphere/skyViewLUT.frag");
        skyViewShader.Activate();
        skyViewShader.setInt("transmittanceLUT", 0);
        skyViewShader.setInt("multipleScatteredLUT", 1);
        skyViewShader.setVec2("iChannelResolution0", glm::vec2(width, height));
        skyViewShader.setVec2("iChannelResolution1", glm::vec2(width, height));
        skyViewShader.setFloat("iTime", currentFrame);
        skyViewShader.setVec2("iMouse", glm::vec2(0.0, 0.0));
        skyViewShader.setVec3("camPos", camera.getPosition());
        skyViewShader.setMat4("viewMatrix", camera.getViewMatrix());
        skyViewShader.setMat4("projectionMatrix", camera.getProjectionMatrix());

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, transmittanceLUT.texture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, multipleScatteredLUT.texture);
        Utils::OpenGL::Draw::drawQuad();
        skyViewLUT.Unbind();

        atmosphereScene.Bind();
        glViewport(0, 0, AppWindow::width, AppWindow::height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        Shader atmosphereShader("Shaders/atmospheric.vert", "Shaders/skyAtmosphere/atmosphere.frag");
        atmosphereShader.Activate();
        atmosphereShader.setInt("skyLUT", 0);
        atmosphereShader.setVec2("iChannelResolution", glm::vec2(width, height));
        atmosphereShader.setFloat("iTime", currentFrame);
        atmosphereShader.setVec2("iMouse", glm::vec2(0.0, 0.0));
        atmosphereShader.setVec3("camPos", camera.getPosition());
        atmosphereShader.setMat4("viewMatrix", camera.getViewMatrix());
        atmosphereShader.setMat4("projectionMatrix", camera.getProjectionMatrix());

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, skyViewLUT.texture);
        Utils::OpenGL::Draw::drawQuad();
        atmosphereScene.Unbind();

        if (guiOn) {
            guiController.start();

            if (ImGui::Begin("control")) {
                ImGui::Button("updated skybox");
                ImGui::BeginChild("gBuffers textures");
                ImGui::DragFloat("Falling speed", &speed, 0.01, -10.0, 10.0);
                ImGui::DragInt("Num Instances", &numRender, numInstances / 100.0, 0, numInstances, 0, true);
                if (ImGui::DragFloat3("Spawn Area", glm::value_ptr(particleControl.spawnArea), 0.1, 0, 1000.0, 0, true)) {
                    particleRenderer.clear();
                    particleRenderer.init(particleControl);
                }
                ImGui::Checkbox("Pause", &pause);
                ImGui::SameLine();
                if (ImGui::Button("Reset")) {
                    particleRenderer.reset();
                }
                ImVec2 wsize = ImGui::GetWindowSize();
                ImGui::Image((ImTextureID)transmittanceLUT.texture, wsize, ImVec2(0, 1), ImVec2(1, 0));
                ImGui::Image((ImTextureID)multipleScatteredLUT.texture, wsize, ImVec2(0, 1), ImVec2(1, 0));
                ImGui::Image((ImTextureID)skyViewLUT.texture, wsize, ImVec2(0, 1), ImVec2(1, 0));
                ImGui::EndChild();
                ImGui::End();
            }

            if (ImGui::Begin("Application window")) {
                ImGui::BeginChild("Child");
                ImVec2 wsize = ImGui::GetWindowSize();
                int wWidth = static_cast<int>(ImGui::GetWindowWidth());
                int wHeight = static_cast<int>(ImGui::GetWindowHeight());
                camera.updateViewResize(wWidth, wHeight);
                ImGui::Image((ImTextureID)colorPassFBO.texture, wsize, ImVec2(0, 1), ImVec2(1, 0));
                //ImGui::Image((ImTextureID)finalSceneFBO.texture, wsize, ImVec2(0, 1), ImVec2(1, 0));
                //ImGui::Image((ImTextureID)atmosphereScene.texture, wsize, ImVec2(0, 1), ImVec2(1, 0));
                if (ImGui::IsItemHovered())
                    camera.processInput(AppWindow::window);
                ImGui::EndChild();
                ImGui::End();
            }

            guiController.end();
        }

        glfwPollEvents();
        glfwSwapBuffers(AppWindow::window);
    }
*/

	return 0;
}

int DeferredIBLDemo::run()
{
	return show_demo();
}
