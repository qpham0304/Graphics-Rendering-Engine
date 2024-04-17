
#include "../../SceneRenderer.h"
#include <memory>

int main()
{
	ImGuiController imguiController;
	OpenGLController openglController;

	//SceneRenderer renderer(imguiController, openglController);
	SceneRenderer renderer;

	renderer.renderScene();
}