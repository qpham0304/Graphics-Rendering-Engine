#include "EditorLayer.h"

#include "../../core/features/AppWindow.h"
#include "../features/Timer.h"
#include "../../graphics/utils/Utils.h"
#include "../../src/apps/particle-demo/ParticleDemo.h"
#include "../../src/apps/deferred-IBL-demo/deferredIBL_demo.h"
#include "../../src/apps/volumetric-light/VolumetricLightDemo.h"
#include "../../src/events/EventManager.h"
#include "../layers/AppLayer.h"
#include "../layers/BloomLayer.h"

EditorLayer::EditorLayer()
{
	onAttach();
}

void EditorLayer::init(ImGuiController& controller)
{
	editorCamera.Init(AppWindow::width, AppWindow::height, glm::vec3(1.0, 0.0, 0.0), glm::vec3(1.0));
	guiController = &controller;
	sceneManager.addScene("default");
	sceneManager.scenes["default"]->addLayer(new ParticleDemo("demo"));
	//sceneManager.scenes["default"]->addLayer(new AppLayer("app"));
}

void EditorLayer::onAttach()
{
	AsyncEvent addComponentEvent;
	auto func = [](Event& event) -> void {
		Component reimu("Models/reimu/reimu.obj");
		SceneManager::addComponent(reimu);
		};
	eventManager.Queue(addComponentEvent, func);

	AsyncEvent addComponentEvent1;
	auto func1 = [](Event& event) -> void {
		Component reimu("Models/sponza/sponza.obj");
		SceneManager::addComponent(reimu);
		};
	eventManager.Queue(addComponentEvent1, func1);

	AsyncEvent addComponentEvent2;
	auto func2 = [](Event& event) -> void {
		Component reimu("Models/aru/aru.gltf");
		SceneManager::addComponent(reimu);
		};
	eventManager.Queue(addComponentEvent2, func2);

	Component* component = nullptr;
	bool* ready = nullptr;
	ModelLoadAsyncEvent addComponentEvent3(component);
	auto func3 = [&component, &ready](Event& event) -> void {
		ModelLoadAsyncEvent& e = static_cast<ModelLoadAsyncEvent&>(event);
		component = new Component("Models/aru/aru.gltf");
		ready = &e.isCompleted;
		};
}

void EditorLayer::onDetach()
{

}

void EditorLayer::onUpdate()
{
	editorCamera.onUpdate();

}

void EditorLayer::onGuiUpdate()
{
	guiController->render();

	ImGui::Begin("test board");
	ImGui::BeginChild("test child");
	ImVec2 wsize = ImGui::GetWindowSize();
	auto fbo = LayerManager::getFrameBuffer("ParticleDemo");
	if (fbo) {
		ImGui::Image((ImTextureID)fbo->texture, wsize, ImVec2(0, 1), ImVec2(1, 0));
	}
	ImGui::EndChild();
	ImGui::End();

	std::string id;
	if (ImGui::Begin("right side bar")) {
		if (ImGui::Button("add demo layer")) {
			id = "demo " + std::to_string(sceneManager.scenes["default"]->layerManager.size());
			sceneManager.scenes["default"]->addLayer(new ParticleDemo(id.c_str()));
			//layerManager.AddLayer(new DeferredIBLDemo("Deferred IBL Demo"));
		}
		if (ImGui::Button("add bloom layer")) {
			id = "bloom " + std::to_string(sceneManager.scenes["default"]->layerManager.size());
			sceneManager.scenes["default"]->addLayer(new BloomLayer(id.c_str()));
		}
		if (ImGui::Button("remove layer")) {	//TODO: should be able to delete selected layers
			sceneManager.scenes["default"]->removeLayer(1);
		}
		ImGui::End();
	}
}

void EditorLayer::onEvent(Event& event)
{

}



