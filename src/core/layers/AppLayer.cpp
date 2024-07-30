#include "AppLayer.h"

AppLayer::AppLayer(const std::string& name) : Layer(name)
{
	
}

void AppLayer::OnAttach()
{
	//applicationFBO.reset(new FrameBuffer(SceneRenderer::width, SceneRenderer::height));
	//camera.reset(new Camera(SceneRenderer::width, SceneRenderer::height, glm::vec3(1.0, 0.0, 0.0), glm::vec3(1.0)));
	applicationFBO.Init(
		SceneRenderer::width,
		SceneRenderer::height,
		GL_RGBA32F,
		GL_RGBA,
		GL_FLOAT,
		nullptr
	);
	camera.Init(SceneRenderer::width, SceneRenderer::height, glm::vec3(1.0, 0.0, 0.0), glm::vec3(1.0));
	skybox.reset(new SkyboxComponent());
}

void AppLayer::OnDetach()
{

}

void AppLayer::OnUpdate()
{
	camera.onUpdate();
	camera.processInput(SceneRenderer::window);
	applicationFBO.Bind();
	glViewport(0.0, 0.0, SceneRenderer::width, SceneRenderer::height);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // RGBA
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Shader lightShader("Shaders/light.vert", "Shaders/light.frag");
	lightShader.Activate();
	lightShader.setMat4("mvp", camera.getMVP());
	lightShader.setMat4("matrix", glm::mat4(1.0));
	lightShader.setVec3("lightColor", glm::vec3(0.7, 0.8, 0.5));
	Utils::OpenGL::Draw::drawCube();
	skybox->render(camera);
	applicationFBO.Unbind();
}

void AppLayer::OnEvent(Event& event)
{

}

const int AppLayer::GetTextureID()
{
	return applicationFBO.texture;
}
