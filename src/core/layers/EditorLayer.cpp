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
	editorCamera.Init(AppWindow::width, AppWindow::height, glm::vec3(1.0, 0.0, 0.0), glm::vec3(1.0));
	guiController = &controller;
	sceneManager.addScene("default");
	//sceneManager.scenes["default"]->addLayer(new ParticleDemo("demo"));
	//sceneManager.scenes["default"]->addLayer(new AppLayer("app"));
	sceneManager.scenes["default"]->addLayer(new DeferredIBLDemo("demo"));
}

void EditorLayer::onAttach()
{
	
}

void EditorLayer::onDetach()
{

}

void EditorLayer::onUpdate()
{
	editorCamera.onUpdate();
}

void displayMatrix(glm::mat4& matrix) {
    for (int row = 0; row < 4; ++row) {
		for (int col = 0; col < 4; ++col) {
			ImGui::PushID(row * 4 + col);  // unique ID for each input
			ImGui::PushItemWidth(100.0);
			ImGui::InputFloat(("##m" + std::to_string(row) + std::to_string(col)).c_str(), &matrix[row][col], 0.0f, 0.0f, "%.3f");
			ImGui::PopItemWidth();
			ImGui::PopID();
			if (col < 3) {
				ImGui::SameLine();
			}
        }
    }
}

void EditorLayer::onGuiUpdate()
{
	guiController->render();

	if (ImGui::Button("add thread tasks")) {
		mockThreadTasks();
	}

	ImGui::Begin("Scenes");
	for (auto& [name, scene] : sceneManager.scenes) {
		Timer("component event", true);
		if (ImGui::Button("add entity")) {
			Console::println(scene->addEntity("myentity"));
		}

		//if(ImGui::TreeNodeEx(scene->getName().c_str())) {
		//	for (auto& layer : scene->layerManager) {
		//		ImGui::TreeNodeEx(layer->GetName().c_str());
		//	}
		//}

		try {
			for (auto& [uuid, entity] : scene->entities) {
				if (ImGui::Button("remove entity")) {
					scene->removeEntity(uuid);
				}
				ImGui::SameLine();
				if (ImGui::Button("addComponent")) {
					std::string path = Utils::Window::WindowFileDialog();
					
					if (!path.empty()) {
						entity.addComponent<ModelComponent>();
						//std::scoped_lock<std::mutex> lock(SceneManager::getInstance().modelsLock);
						
						auto func = [&entity](AsyncEvent& event) {
							ModelComponent& component = entity.getComponent<ModelComponent>();
							component.path = "Loading...";
							SceneManager::getInstance().addModel(event.id.c_str());
							if (component.path != event.id) {
								component.model = SceneManager::getInstance().models[event.id];
							}

							// if another thread deletes the same model before this add, reset the model component's pointer
							if (auto lockedModel = component.model.lock()) {	
								if (lockedModel.get() == SceneManager::getInstance().models[event.id].get()) {
									Console::println("match address");
								}
								Console::println("num references", lockedModel.use_count());
								Console::println("path", lockedModel.get());
								Console::println("path", SceneManager::getInstance().models[event.id].get());
							}
							else {
								component.reset();
							}
							component.path = event.id;
						};
						AsyncEvent event(path);
						EventManager::getInstance().Queue(event, func);
					}
				}


				ImGui::SameLine();
				if (ImGui::Button("addAnimationComponent")) {
					entity.addComponent<AnimationComponent>();
				}
				//ImGui::SameLine();
				//if (ImGui::Button("addMeshComponent")) {
				//	entity.addComponent<MeshComponent>();
				//}
				if (ImGui::TreeNodeEx(uuid.c_str())) {
					auto transform = entity.getComponent<TransformComponent>();
					displayMatrix(transform.model);
					ImGui::Text(entity.getComponent<NameComponent>().name.c_str());
					if (entity.hasComponent<ModelComponent>()) {
						ImGui::Text(entity.getComponent<ModelComponent>().path.c_str());
					}
				}
			}
		}

		catch (std::runtime_error e) {
			Console::println(e.what());
		}
	}

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



