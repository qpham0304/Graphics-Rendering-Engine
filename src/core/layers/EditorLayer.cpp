#include "EditorLayer.h"

#include "../../core/features/AppWindow.h"
#include "camera.h"
#include "../features/Timer.h"
#include "../../graphics/utils/Utils.h"
#include "../../src/apps/particle-demo/ParticleDemo.h"
#include "../../src/apps/deferred-IBL-demo/deferredIBL_demo.h"
#include "../../src/apps/volumetric-light/VolumetricLightDemo.h"
#include "../../src/events/EventManager.h"
#include "../layers/AppLayer.h"
#include "../layers/BloomLayer.h"
#include "../components/MComponent.h"


void EditorLayer::mockThreadTasks()
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
}

EditorLayer::EditorLayer()
{
	onAttach();
}

void EditorLayer::init(ImGuiController& controller)
{
	guiController = &controller;
	guiController->useDarkTheme();
	editorCamera.Init(AppWindow::width, AppWindow::height, glm::vec3(1.0, 0.0, 0.0), glm::vec3(1.0));
	sceneManager.addScene("default");
	//sceneManager.getScene("default")->addLayer(new ParticleDemo("demo"));
	//sceneManager.getScene("default")->addLayer(new AppLayer("app"));
	sceneManager.getScene("default")->addLayer(new DeferredIBLDemo("demo"));
}

void EditorLayer::onAttach()
{
	auto addModelFunc = [](Event& event) {
		ModelLoadEvent& e = static_cast<ModelLoadEvent&>(event);
		if (!e.entity.hasComponent<ModelComponent>()) {
			e.entity.addComponent<ModelComponent>();
		}
		ModelComponent& component = e.entity.getComponent<ModelComponent>();
		component.path = "Loading...";
		bool canAdd = SceneManager::getInstance().addModel(e.path.c_str());
		if (component.path != e.path && canAdd) {
			component.model = SceneManager::getInstance().models[e.path];
			component.path = e.path;
		}
		else {
			ImGui::OpenPopup("Failed to load file, please check the format");
			component.reset();
		}
	};
	EventManager::getInstance().Subscribe(EventType::ModelLoadEvent, addModelFunc);
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
		Scene* scene = sceneManager.getScene("default");
		if (ImGui::Button("add demo layer")) {
			id = "demo " + std::to_string(scene->layerManager.size());
			scene->addLayer(new ParticleDemo(id.c_str()));
			//layerManager.AddLayer(new DeferredIBLDemo("Deferred IBL Demo"));
		}
		if (ImGui::Button("add bloom layer")) {
			id = "bloom " + std::to_string(scene->layerManager.size());
			scene->addLayer(new BloomLayer(id.c_str()));
		}
		if (ImGui::Button("remove layer")) {	//TODO: should be able to delete selected layers
			scene->removeLayer(1);
		}
		ImGui::End();
	}
}

void EditorLayer::onEvent(Event& event)
{

}



