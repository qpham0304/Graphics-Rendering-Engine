
#include "../../SceneRenderer.h"

void demo() {
    Camera camera(SceneRenderer::width, SceneRenderer::height, glm::vec3(-6.5f, 3.5f, 8.5f), glm::vec3(0.5, -0.2, -1.0f));
    Shader modelShader("Shaders/default.vert", "Shaders/default.frag");
    
    glm::mat4 modelMatrix(1.0f);
    std::string lucy = "C:\\Users\\tomor\\Desktop\\Projects\\Resources\\3d models\\lucy.gltf";
    Model r8("Models/r8/r8.fbx");
    ModelComponent r8Component("Models/r8/r8.fbx");

    glm::vec3 lightPos(glm::vec3(0.5f, 4.5f, 5.5f));
    glm::vec4 lightcolor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    float ambient = 1.0f;
    float sampleRadius = 2;

    float near_plane = 1.0f, far_plane = 12.5f;
    glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
    glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
    glm::mat4 lightMVP = lightProjection * lightView;
    
    int frameCounter = 0;
    float lastTime = 0.0f;

    Shader shadowMapShader("Shaders/shadowMap.vert", "Shaders/shadowMap.frag");
    Shader debugDepthQuad("src/apps/shadow-map/debug.vert", "src/apps/shadow-map/debug.frag");


    DepthMap depthMap;
    while (!glfwWindowShouldClose(SceneRenderer::window)) {
        // Clear the color buffer
        glViewport(0, 0, SceneRenderer::width, SceneRenderer::height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float currentTime = static_cast<float>(glfwGetTime());
        float timeDiff = currentTime - lastTime;
        frameCounter++;

        if (timeDiff >= 1 / 2) {
            std::string FPS = std::to_string((1.0 / timeDiff) * frameCounter);
            std::string ms = std::to_string((timeDiff / frameCounter) * 1000);
            std::string updatedTitle = "Graphic Engine - " + FPS + "FPS / " + ms + "ms";
            glfwSetWindowTitle(SceneRenderer::window, updatedTitle.c_str());
            lastTime = currentTime;
            frameCounter = 0;
        }

        camera.processInput(SceneRenderer::window);
        camera.cameraViewUpdate();
     
        
        depthMap.Bind();
        glViewport(0, 0, depthMap.SHADOW_WIDTH, depthMap.SHADOW_HEIGHT);
        glClear(GL_DEPTH_BUFFER_BIT);
        glActiveTexture(GL_TEXTURE0);
        
        lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
        lightMVP = lightProjection * lightView;
        Light light = Light(lightPos, lightcolor, ambient, lightMVP, sampleRadius);
        shadowMapShader.Activate();
        shadowMapShader.setMat4("mvp", lightMVP);
        
        //for (int i = 0; i < 100; i++) {
        //    modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f + i, 1.0f, 1.0f));
        //    shadowMapShader.setBool("hasAnimation", false);
        //    shadowMapShader.setMat4("matrix", modelMatrix);
        //    r8.Draw(shadowMapShader);
        //}

        depthMap.Unbind();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // RGBA
        glViewport(0, 0, SceneRenderer::width, SceneRenderer::height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        r8Component.render(camera, light);

        // Process events
        glfwPollEvents();

        // Swap the front and back buffers
        glfwSwapBuffers(SceneRenderer::window);
    }
}

int main()
{
	try {
		SceneRenderer::init(PLATFORM_OPENGL);
		SceneRenderer::start("OpenGL Game Engine");
		SceneRenderer::renderScene();
        //demo();

		SceneRenderer::end();
	}
	catch (const std::runtime_error& e) {
		std::cerr << "Exception: " << e.what() <<std::endl;
	}
}