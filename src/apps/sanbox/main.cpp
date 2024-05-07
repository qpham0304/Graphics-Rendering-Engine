
#include "../../SceneRenderer.h"

void demo() {
    Camera camera(SceneRenderer::width, SceneRenderer::height, glm::vec3(-6.5f, 3.5f, 8.5f), glm::vec3(0.5, -0.2, -1.0f));
    Shader modelShader("Shaders/default.vert", "Shaders/default.frag");


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

    SkyboxComponent skybox;
    Model model("Models/reimu/reimu.obj");
    Shader shader("Shaders/cubemap.vert", "Shaders/cubemap.frag");

    DepthMap depthMap;
    glEnable(GL_DEPTH_TEST);
    while (!glfwWindowShouldClose(SceneRenderer::window)) {
        // Clear the color buffer
        glViewport(0, 0, SceneRenderer::width, SceneRenderer::height);
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f); // RGBA
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        camera.cameraViewUpdate();
        camera.processInput(SceneRenderer::window);

        shader.Activate();
        shader.setMat4("model", glm::mat4(1.0f));
        shader.setMat4("mvp", camera.getMVP());
        shader.setVec3("camPos", camera.getPosition());
        model.Draw(shader);


        skybox.render(camera);

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