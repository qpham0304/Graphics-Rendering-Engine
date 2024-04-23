
#include "../../SceneRenderer.h"

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