
#include "../../graphics/renderer/SceneRenderer.h"
#include "../image-based-rendering/pbr_demo.h"
#include "../deferred-rendering/deferred_render_demo.h"
#include "../area-light/area_light_demo.h"
#include "../SSAO-demo/SSAO_demo.h"
#include "../volumetric-light/volumetricLightDemo.h"
#include "../SSR-demo/SSR_demo.h"
#include "../SSR-view-demo/SSR_view_demo.h"
#include "../deferred-IBL-demo/deferredIBL_demo.h"
#include "../ray-tracing/BasicRayTracing.h"
#include "../particle-demo/ParticleDemo.h"
#include "../../core/Application.h"

int main()
{
	Application app;
	//App app
	//while(app.running)
	//app.close
	//while (true) {
		try {
			SceneRenderer::RunFunc demo0 = DemoPBR::run;
			SceneRenderer::RunFunc demo1 = AreaLightDemo::run;
			SceneRenderer::RunFunc demo2 = DeferredRender::run;
			SceneRenderer::RunFunc demo3 = VolumetricLightDemo::run;
			SceneRenderer::RunFunc demo4 = SSAO_Demo::run;
			SceneRenderer::RunFunc demo5 = SSR_demo::run;
			SceneRenderer::RunFunc demo6 = SSR_view_demo::run;
			SceneRenderer::RunFunc demo7 = deferredIBL_demo::run;
			SceneRenderer::RunFunc demo8 = BasicRayTracing::run;
			SceneRenderer::RunFunc demo9 = ParticleDemo::run;

			// Trying to be dumb, ignore this
			std::vector<int(*)()> list;
			list.push_back(demo0);
			list.push_back(demo1);
			list.push_back(demo2);
			list.push_back(demo3);
			list.push_back(demo4);
			list.push_back(demo5);
			list.push_back(demo6);
			list.push_back(demo7);
			list.push_back(demo8);
			list.push_back(demo9);

			//SceneRenderer::init(PLATFORM_OPENGL);
			//SceneRenderer::start("OpenGL Game Engine");
			//SceneRenderer::renderScene(list[9]);
			//SceneRenderer::end();
			app.run();


		}
		catch (const std::runtime_error& e) {
			std::cerr << "Exception: " << e.what() << std::endl;
		}
	//}

}