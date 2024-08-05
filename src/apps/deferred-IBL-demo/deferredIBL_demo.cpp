#include "deferredIBL_demo.h"
#include "ParticleGeometry.h"

DeferredIBLDemo::DeferredIBLDemo(const std::string& name) : AppLayer(name)
{
    particleRenderer.init(particleControl);
    lights.push_back(Light(glm::vec3(2.0, 0.5, -30.0), glm::vec4(1000.0f, 1000.0f, 3000.0f, 1.0f)));
    lights.push_back(Light(glm::vec3(-2.0, 0.5, 2.0), glm::vec4(200.0f, 100.0f, 100.0f, 1.0f)));

    pbrShader.reset(new Shader("Shaders/default-2.vert", "Shaders/default-2.frag"));
    equirectangularToCubemapShader.reset(new Shader("Shaders/cubemap-hdr.vert", "Shaders/equireRectToCubemap.frag"));
    irradianceShader.reset(new Shader("Shaders/cubemap-hdr.vert", "Shaders/irradianceConvolution.frag"));
    backgroundShader.reset(new Shader("Shaders/background.vert", "Shaders/background.frag"));
    prefilterShader.reset(new Shader("Shaders/cubemap-hdr.vert", "Shaders/prefilter.frag"));
    brdfShader.reset(new Shader("Shaders/brdf.vert", "Shaders/brdf.frag"));


    Component helmetModel("Models/DamagedHelmet/gltf/DamagedHelmet.gltf");
    //Component terrain("Models/mountain_asset_canadian_rockies_modular/scene.gltf");
    Component terrain("Models/death-valley-terrain/scene.gltf");
    Texture tex("Textures/squish.png", "colorScene");

    SceneManager::addComponent(helmetModel);
    SceneManager::addComponent(terrain);
    for (const auto& [id, component_ptr] : SceneManager::components) {
        components.push_back(component_ptr.get());
    }


    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

    //---------------envcubemap set up---------------//
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

    //// convert HDR equirectangular environment map to cubemap equivalent
    equirectangularToCubemapShader->Activate();
    equirectangularToCubemapShader->setInt("equirectangularMap", 0);
    equirectangularToCubemapShader->setMat4("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdrTexture);

    glViewport(0, 0, 512, 512); // don't forget to configure the viewport to the capture dimensions.
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

    for (unsigned int i = 0; i < 6; ++i)
    {
        equirectangularToCubemapShader->setMat4("view", captureViews[i]);
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
    irradianceShader->Activate();
    irradianceShader->setInt("environmentMap", 0);
    irradianceShader->setMat4("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemapTexture);

    glViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    for (unsigned int i = 0; i < 6; ++i)
    {
        irradianceShader->setMat4("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Utils::OpenGL::Draw::drawCube(cubeVAO, cubeVBO);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //------------------------------------------------------------//

    //-----------------prefilter map---------------------//
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

    prefilterShader->Activate();
    prefilterShader->setInt("environmentMap", 0);
    prefilterShader->setMat4("projection", captureProjection);
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
        prefilterShader->setFloat("roughness", roughness);
        for (unsigned int i = 0; i < 6; ++i)
        {
            prefilterShader->setMat4("view", captureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            Utils::OpenGL::Draw::drawCube(cubeVAO, cubeVBO);
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //-----------------------------------------------------------//

    //-----------------------TLU map-----------------------------//
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
    brdfShader->Activate();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    Utils::OpenGL::Draw::drawQuad();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void myfunc(Event& event)
{
    MouseMoveEvent& mouseEvent = static_cast<MouseMoveEvent&>(event);
    SceneManager::cameraController->processMouse(mouseEvent.window);
    std::cout << "moving... 3" << mouseEvent.GetName() << std::endl;
    //event.Handled = true;
}

void DeferredIBLDemo::OnAttach()
{
    AppLayer::OnAttach();
    SceneManager::cameraController = &camera;
    camera = *SceneManager::cameraController;
    EventManager& eventManager = EventManager::getInstance();
    eventManager.Subscribe(EventType::MouseMoved, [this](Event& event) {
        this->OnEvent(event);
    });
    //eventManager.Subscribe(
    //    EventType::MouseMoved, std::bind(&DeferredIBLDemo::OnEvent, this, std::placeholders::_1)
    //);
    LayerManager::addFrameBuffer("DeferredIBLDemo", applicationFBO);
}

void DeferredIBLDemo::OnDetach()
{
    AppLayer::OnDetach();
}

void DeferredIBLDemo::OnUpdate()
{
    pbrShader->Activate();
    pbrShader->setInt("albedoMap", 0);
    pbrShader->setInt("normalMap", 1);
    pbrShader->setInt("metallicMap", 2);
    pbrShader->setInt("roughnessMap", 3);
    pbrShader->setInt("aoMap", 4);
    pbrShader->setInt("emissiveMap", 5);
    pbrShader->setInt("irradianceMap", 6);
    pbrShader->setInt("prefilterMap", 7);
    pbrShader->setInt("brdfLUT", 8);

    backgroundShader->Activate();
    backgroundShader->setInt("environmentMap", 0);

    applicationFBO.Bind();
    glViewport(0, 0, AppWindow::width, AppWindow::height);
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    pbrShader->Activate();
    pbrShader->setMat4("matrix", glm::mat4(1.0f));
    pbrShader->setMat4("mvp", camera.getMVP());
    pbrShader->setVec3("camPos", camera.getPosition());
    pbrShader->setBool("gamma", true);

    // bind pre-computed IBL data
    glActiveTexture(GL_TEXTURE0 + 6);
    glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
    glActiveTexture(GL_TEXTURE0 + 7);
    glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
    glActiveTexture(GL_TEXTURE0 + 8);
    glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);

    pbrShader->setMat4("matrix", components[0]->getModelMatrix());
    pbrShader->setBool("hasAnimation", false);
    pbrShader->setBool("hasEmission", true);
    pbrShader->setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(components[0]->getModelMatrix()))));
    components[0]->model_ptr->Draw(*pbrShader);

    pbrShader->setBool("hasAnimation", false);
    pbrShader->setBool("hasEmission", false);
    components[1]->translate(glm::vec3(0.0f, -10.0f, -15.0f));
    components[1]->scale(glm::vec3(10.0f));
    glm::mat4 terrainModelMatrix = glm::rotate(components[1]->getModelMatrix(), glm::radians(180.0f), glm::vec3(1.0, 0.0, 0.0));
    pbrShader->setMat4("matrix", terrainModelMatrix);
    pbrShader->setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(terrainModelMatrix))));
    components[1]->model_ptr->Draw(*pbrShader);

    for (unsigned int i = 0; i < lights.size(); ++i)
    {
        pbrShader->setVec3("lightPositions[" + std::to_string(i) + "]", lights[i].position);
        pbrShader->setVec3("lightColors[" + std::to_string(i) + "]", lights[i].color);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, lights[i].position);
        model = glm::scale(model, glm::vec3(0.5f));
        pbrShader->setMat4("matrix", model);
        pbrShader->setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
        Utils::OpenGL::Draw::drawSphere(sphereVAO, indexCount);
    }
    //particleRenderer.render(*lightShader, camera, numRender, speed, pause);

    skybox->updateTexture(envCubemapTexture);
    skybox->render(camera);

    applicationFBO.Unbind();
}

void DeferredIBLDemo::OnGuiUpdate()
{
    AppLayer::OnGuiUpdate();
}

void DeferredIBLDemo::OnEvent(Event& event)
{
    MouseMoveEvent& mouseEvent = static_cast<MouseMoveEvent&>(event);
    SceneManager::cameraController->processMouse(mouseEvent.window);
    //SceneManager::cameraController->mouse_callback(mouseEvent.window, mouseEvent.m_x, mouseEvent.m_y);
    std::cout << "moving... 3" << mouseEvent.GetName() << std::endl;
    //event.Handled = true;
}

int DeferredIBLDemo::show_demo()
{
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

    SkyboxComponent skybox;

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

    Texture tex("Textures/squish.png", "colorScene");

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

	return 0;
}

int DeferredIBLDemo::run()
{
	return show_demo();
}
