
#include "../../SceneRenderer.h"
#include "../image-based-rendering/pbr_demo.h"

int main()
{
	try {
		SceneRenderer::init(PLATFORM_OPENGL);
		SceneRenderer::start("OpenGL Game Engine");
		//SceneRenderer::renderScene();
        DemoPBR::run();
		SceneRenderer::end();
	}
	catch (const std::runtime_error& e) {
		std::cerr << "Exception: " << e.what() <<std::endl;
	}
}