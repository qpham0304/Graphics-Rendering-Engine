
#include "pbr_demo.h"
unsigned int sphereVAO = 0;
unsigned int indexCount;

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

    Shader shader("Shaders/default-2.vert", "Shaders/default-2.frag");
    Model model("Models/planet/planet.obj");
    SkyboxComponent skybox;
    MaterialPBR materialPBR = MaterialPBR(glm::vec3(0.5, 0.5, 0.5), 1.0, 1.0, 1.0);



    int nrRows = 7;
    int nrColumns = 7;
    float spacing = 2.5;
    unsigned int sphereVAO = 0;
    unsigned int indexCount;

    glEnable(GL_DEPTH_TEST);
    while (!glfwWindowShouldClose(SceneRenderer::window)) {
        // Clear the color buffer
        glViewport(0, 0, SceneRenderer::width, SceneRenderer::height);
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f); // RGBA
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        camera.cameraViewUpdate();
        camera.processInput(SceneRenderer::window);

        shader.Activate();
        shader.setMat4("matrix", glm::mat4(1.0f));
        shader.setMat4("mvp", camera.getMVP());
        shader.setVec3("camPos", camera.getPosition());

        shader.Activate();
        shader.setInt("albedoMap", 0);
        shader.setInt("normalMap", 1);
        shader.setInt("metallicMap", 2);
        shader.setInt("roughnessMap", 3);
        shader.setInt("aoMap", 4);
        shader.setBool("gamma", true);

        for (int i = 0; i < textures.size(); i++) {
            textures[i].texUnit(shader, textures[i].type, i);
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
                shader.setMat4("matrix", model);
                shader.setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
                Utils::Draw::drawSphere(sphereVAO, indexCount);
            }
        }


        for (unsigned int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i)
        {
            glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
            newPos = lightPositions[i];
            shader.setVec3("lightPositions[" + std::to_string(i) + "]", newPos);
            shader.setVec3("lightColors[" + std::to_string(i) + "]", lightColors[i]);

            model = glm::mat4(1.0f);
            model = glm::translate(model, newPos);
            model = glm::scale(model, glm::vec3(0.5f));
            shader.setMat4("matrix", model);
            shader.setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
            Utils::Draw::drawSphere(sphereVAO, indexCount);
        }


        skybox.render(camera);


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