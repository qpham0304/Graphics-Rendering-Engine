
#include "../../SceneRenderer.h"
#include "../image-based-rendering/pbr_demo.h"
#include "../deferred-rendering/deferred_render_demo.h"
#include "../area-light/area_light_demo.h"
#include "../SSAO-demo/SSAO_demo.h"
#include "../volumetric-light/volumetricLightDemo.h"
#include "../SSR-demo/SSR_demo.h"
#include "../SSR-view-demo/SSR_view_demo.h"
#include "../deferred-IBL-demo/deferredIBL_demo.h"

int main()
{
	try {
		SceneRenderer::init(PLATFORM_OPENGL);
		SceneRenderer::start("OpenGL Game Engine");
		//glfwSwapInterval(0);
		//SceneRenderer::renderScene();
        //DemoPBR::run();
		//AreaLightDemo::run();
		//DeferredRender::run();
		//VolumetricLightDemo::run();
		//SSAO_Demo::run();
		//SSR_demo::run();
		//SSR_view_demo::run();
		deferredIBL_demo::run();
		SceneRenderer::end();
	}
	catch (const std::runtime_error& e) {
		std::cerr << "Exception: " << e.what() <<std::endl;
	}
}