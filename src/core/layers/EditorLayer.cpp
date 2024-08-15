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
#include "../components/cameracomponent.h"

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

void EditorLayer::renderGuizmo()
{
	glm::vec3 translateVector(0.0f, 0.0f, 0.0f);
	glm::vec3 scaleVector(1.0f, 1.0f, 1.0f);

	float viewManipulateRight = ImGui::GetIO().DisplaySize.x;
	float viewManipulateTop = 0;

	auto v = &SceneManager::cameraController->getViewMatrix()[0][0];
	auto p = glm::value_ptr(SceneManager::cameraController->getProjectionMatrix());
	Scene* scene = SceneManager::getInstance().getScene("default");
	std::vector<Entity> selectedEntities = scene->getSelectedEntities();

	if (!selectedEntities.empty()) {
		TransformComponent& transformComponent = selectedEntities[0].getComponent<TransformComponent>();

		glm::mat4& transform = transformComponent.getModelMatrix();

		ImGuizmo::SetOrthographic(false);
		ImGuizmo::SetDrawlist();
		float wd = (float)ImGui::GetWindowWidth();
		float wh = (float)ImGui::GetWindowHeight();

		ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, wd, wh);
		glm::mat4 identity(1.0f);

		if (drawGrid)
			ImGuizmo::DrawGrid(v, p, glm::value_ptr(identity), 100.f);

		bool res = ImGuizmo::Manipulate(v, p, (ImGuizmo::OPERATION)GuizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform));
		viewManipulateRight = ImGui::GetWindowPos().x + wd;
		viewManipulateTop = ImGui::GetWindowPos().y;
		ImGuizmo::ViewManipulate(v, 5.0f, ImVec2(viewManipulateRight - 128, viewManipulateTop), ImVec2(128, 128), 0x10101010);

		if (ImGuizmo::IsUsing()) {
			GuizmoActive = true;
			glm::vec3 translation, rotation, scale;
			Utils::Math::DecomposeTransform(transform, translation, rotation, scale);
			glm::vec3 deltaRotation = rotation - transformComponent.rotateVec;

			transformComponent.translateVec = translation;
			transformComponent.rotateVec += deltaRotation;
			transformComponent.scaleVec = scale;
			//transformComponent.updateTransform();
		}
		else {
			GuizmoActive = false;
		}
	}
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
	EventManager::getInstance().Subscribe(EventType::ModelLoadEvent, [](Event& event) {
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
		});

	EventManager::getInstance().Subscribe(EventType::MouseMoved, [&](Event& event) {
		MouseMoveEvent& mouseEvent = static_cast<MouseMoveEvent&>(event);
		if (GuizmoActive && editorActive) {
			mouseEvent.Handled = true;	// block mouse event from other layers
		}
		});

	EventManager::getInstance().Subscribe(EventType::KeyPressed, [&](Event& event) {
		KeyPressedEvent& keyPressedEvent = static_cast<KeyPressedEvent&>(event);
		if (GuizmoActive || editorActive) {
			handleKeyPressed(keyPressedEvent.keyCode);
			keyPressedEvent.Handled = true;	// block keyboard event from other layers
		}
		});


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
	//if(ImGui::Button("addmock data")){
	//	mockThreadTasks();
	//}

	if (ImGui::Begin("Application window")) {
		ImGui::BeginChild("Child");
		ImGui::SetNextItemAllowOverlap();
		renderGuizmo();
		ImGui::EndChild();
		ImGui::End();
	}

	std::string id;
	if (ImGui::Begin("Layers")) {
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

void EditorLayer::handleKeyPressed(int keycode)
{
	if (keycode == KEY_T) {
		GuizmoType = ImGuizmo::OPERATION::TRANSLATE;
	}
	if (keycode == KEY_R) {
		GuizmoType = ImGuizmo::OPERATION::ROTATE;
	}
	if (keycode == KEY_Z) {
		GuizmoType = ImGuizmo::OPERATION::SCALE;
	}
	if (keycode == KEY_DELETE) {
		Scene* scene = SceneManager::getInstance().getScene("default");
		std::vector<Entity> selectedEntities = scene->getSelectedEntities();
		if (!selectedEntities.empty()) {
			scene->removeEntity(selectedEntities[0].getID());
		}
	}
}



