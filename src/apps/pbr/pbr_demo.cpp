
#include "pbr_demo.h"
unsigned int sphereVAO = 0;
unsigned int indexCount;

unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;

int DemoPBR::show_demo() {
    Camera camera(SceneRenderer::width, SceneRenderer::height, glm::vec3(-6.5f, 3.5f, 8.5f), glm::vec3(0.5, -0.2, -1.0f));
    
    glm::vec3 lightPositions[] = {
        glm::vec3(-5.0f,  5.0f, 5.0f),
        glm::vec3(5.0f,  5.0f, 5.0f),
        glm::vec3(-5.0f, -5.0f, 5.0f),
        glm::vec3(5.0f, -5.0f, 5.0f),
    };
    glm::vec4 lightColors[] = {
        glm::vec4(100.0f, 100.0f, 100.0f, 1.0f),
        glm::vec4(100.0f, 100.0f, 100.0f, 1.0f),
        glm::vec4(100.0f, 100.0f, 100.0f, 1.0f),
        glm::vec4(100.0f, 100.0f, 100.0f, 1.0f)
    };

    Texture albedo("pbr/rusted_iron/albedo.png", "diffuse0", "Textures");
    Texture metallic("pbr/rusted_iron/metallic.png", "specular0", "Textures");
    Texture normal("pbr/rusted_iron/normal.png", "normal0", "Textures");
    Texture roughness("pbr/rusted_iron/roughness.png", "", "Textures");
    Texture ao("pbr/rusted_iron/ao.png", "ao0", "Textures");
    std::vector<Texture> textures = {albedo, normal, metallic, roughness, ao};


    unsigned int hdrTexture;
    std::string texRes = Utils::filereader::loadHDRTexture("Textures/hdr/dikhololo_night_1k.hdr", hdrTexture);
    std::cout << texRes << std::endl;

    Shader pbrShader("Shaders/default-2.vert", "Shaders/default-2.frag");
    Shader equirectangularToCubemapShader("Shaders/cubemap-hdr.vert", "Shaders/equireRectToCubemap.frag");
    Shader backgroundShader("Shaders/background.vert", "Shaders/background.frag");
    Shader irradianceShader("Shaders/cubemap-hdr.vert", "Shaders/irradiance-convolution.frag");

    pbrShader.Activate();
    pbrShader.setInt("irradianceMap", 5);

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
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 captureViews[] =
    {
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };

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

        Utils::Draw::drawCube(cubeVAO, cubeVBO); // renders a 1x1 cube
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //------------------------------------------------------------//


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

        Utils::Draw::drawCube(cubeVAO, cubeVBO);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //------------------------------------------------------------//

    glm::mat4 projection = glm::perspective(glm::radians(camera.fov), (float)SceneRenderer::width / SceneRenderer::height, 0.1f, 100.0f);
    pbrShader.Activate();
    pbrShader.setMat4("projection", projection);
    backgroundShader.Activate();
    backgroundShader.setMat4("projection", projection);

    // then before rendering, configure the viewport to the original framebuffer's screen dimensions
    int scrWidth, scrHeight;
    glfwGetFramebufferSize(SceneRenderer::window, &scrWidth, &scrHeight);
    glViewport(0, 0, scrWidth, scrHeight);

    int nrRows = 7;
    int nrColumns = 7;
    float spacing = 2.5;
    unsigned int sphereVAO = 0;
    unsigned int indexCount;

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    while (!glfwWindowShouldClose(SceneRenderer::window)) {
        // Clear the color buffer
        glViewport(0, 0, SceneRenderer::width, SceneRenderer::height);
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f); // RGBA
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        camera.cameraViewUpdate();
        camera.processInput(SceneRenderer::window);

        pbrShader.Activate();
        pbrShader.setMat4("matrix", glm::mat4(1.0f));
        pbrShader.setMat4("mvp", camera.getMVP());
        pbrShader.setVec3("camPos", camera.getPosition());

        pbrShader.setInt("albedoMap", 0);
        pbrShader.setInt("normalMap", 1);
        pbrShader.setInt("metallicMap", 2);
        pbrShader.setInt("roughnessMap", 3);
        pbrShader.setInt("aoMap", 4);
        pbrShader.setBool("gamma", true);

        // bind pre-computed IBL data
        glActiveTexture(GL_TEXTURE0 + 5);
        glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);

        for (int i = 0; i < textures.size(); i++) {
            textures[i].texUnit(pbrShader, textures[i].type, i);
            textures[i].Bind();
        }

        // render rows*column number of spheres with material properties defined by textures (they all have the same material properties)
        glm::mat4 model = glm::mat4(1.0f);
        for (int row = 0; row < nrRows; ++row)
        {
            for (int col = 0; col < nrColumns; ++col)
            {
                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(
                    (float)(col - (nrColumns / 2)) * spacing,
                    (float)(row - (nrRows / 2)) * spacing,
                    0.0f
                ));
                pbrShader.setMat4("matrix", model);
                pbrShader.setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
                Utils::Draw::drawSphere(sphereVAO, indexCount);
            }
        }

        for (unsigned int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i)
        {
            glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
            newPos = lightPositions[i];
            pbrShader.setVec3("lightPositions[" + std::to_string(i) + "]", newPos);
            pbrShader.setVec3("lightColors[" + std::to_string(i) + "]", lightColors[i]);

            model = glm::mat4(1.0f);
            model = glm::translate(model, newPos);
            model = glm::scale(model, glm::vec3(0.5f));
            pbrShader.setMat4("matrix", model);
            pbrShader.setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
            Utils::Draw::drawSphere(sphereVAO, indexCount);
        }

        // render skybox (render as last to prevent overdraw)
        backgroundShader.Activate();
        backgroundShader.setMat4("view", camera.getViewMatrix());
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemapTexture);
        Utils::Draw::drawCube(cubeVAO, cubeVBO);

        // Process events
        glfwPollEvents();

        // Swap the front and back buffers
        glfwSwapBuffers(SceneRenderer::window);
    }

}


int DemoPBR::run()
{
    try {
        show_demo();
        return 0;
    }
    catch (const std::runtime_error& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return -1;
    }
}