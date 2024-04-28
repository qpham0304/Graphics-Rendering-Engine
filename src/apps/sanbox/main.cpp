
#include "../../SceneRenderer.h"

void demo() {
    glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    glm::vec3 lightPos = glm::vec3(0.5f, 4.5f, 5.5f);
    Light light = Light(lightPos, lightColor, 0.5);

    Camera camera(SceneRenderer::width, SceneRenderer::height, glm::vec3(-6.5f, 3.5f, 8.5f), glm::vec3(0.5, -0.2, -1.0f));
    std::string dir = "C:/Users/tomor/Desktop/Projects/C++/OpenGL-projs/comprehensive/";
    Model model((dir + "Models/cube/cube.obj").c_str());
    
    Shader shader((dir + "Shaders/model.vert").c_str(), (dir + "Shaders/model.frag").c_str());
    Texture texture((dir + "Textures/squish.png").c_str());

    while (!glfwWindowShouldClose(SceneRenderer::window)) {
        // Clear the color buffer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // RGBA
        glViewport(0, 0, SceneRenderer::width, SceneRenderer::height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        camera.processInput(SceneRenderer::window);
        camera.cameraViewUpdate();
        if (true) {
            glEnable(GL_CULL_FACE);
            glFrontFace(GL_CCW);
        }

        shader.Activate();
        shader.setMat4("matrix", glm::mat4(1.0f));
        shader.setMat4("mvp", camera.getMVP());
        texture.texUnit(shader, "diffuse0", 0);
        texture.Bind();
        Utils::Draw::drawQuad();
        model.Draw(shader, camera);


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

		SceneRenderer::end();
	}
	catch (const std::runtime_error& e) {
		std::cerr << "Exception: " << e.what() <<std::endl;
	}
}