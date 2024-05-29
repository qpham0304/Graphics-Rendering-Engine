
#include "../../SceneRenderer.h"
#include "../image-based-rendering/pbr_demo.h"
#include "../area-light/area_light_demo.h"

int main()
{
	try {
		SceneRenderer::init(PLATFORM_OPENGL);
		SceneRenderer::start("OpenGL Game Engine");
		//glfwSwapInterval(0);
		//SceneRenderer::renderScene();
        DemoPBR::run();
		//AreaLightDemo::run();
		SceneRenderer::end();
	}
	catch (const std::runtime_error& e) {
		std::cerr << "Exception: " << e.what() <<std::endl;
	}
}