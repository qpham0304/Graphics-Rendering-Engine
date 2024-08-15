#include "../headers/LeftSidebarWidget.h"
#include "../../core/scene/SceneManager.h"
#include "../../events/EventManager.h"

#include <windows.h>
#include <shobjidl.h> 
#include "../../graphics/utils/Utils.h"
#include "../../core/components/MComponent.h"
#include "Texture.h"
#include "../../core/scene/SceneManager.h"
#include "../../core/features/AppWindow.h"

glm::vec3 lightPositions[] = {
    glm::vec3(-5.0f,  5.0f, 5.0f),
    glm::vec3(5.0f,  5.0f, 5.0f),
    glm::vec3(-5.0f, -5.0f, 5.0f),
    glm::vec3(5.0f, -5.0f, 5.0f),
};
glm::vec4 lightColors[] = {
    glm::vec4(300.0f, 300.0f, 300.0f, 1.0f),
    glm::vec4(300.0f, 300.0f, 300.0f, 1.0f),
    glm::vec4(300.0f, 300.0f, 300.0f, 1.0f),
    glm::vec4(300.0f, 300.0f, 300.0f, 1.0f)
};

glm::vec3 translateVec(0.0);


static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow
                                    | ImGuiTreeNodeFlags_OpenOnDoubleClick
                                    | ImGuiTreeNodeFlags_SpanAvailWidth;

static void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f)
{
    ImGuiIO& io = ImGui::GetIO();
    auto boldFont = io.Fonts->Fonts[0];

    ImGui::PushID(label.c_str());

    ImGui::Columns(2);
    ImGui::SetColumnWidth(0, columnWidth);
    ImGui::Text(label.c_str());
    ImGui::NextColumn();

    //ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

    //float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
    float lineHeight = 30.0f;
    ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
    ImGui::PushFont(boldFont);
    if (ImGui::Button("X", buttonSize))
        values.x = resetValue;
    ImGui::PopFont();
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
    ImGui::PopItemWidth();
    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
    ImGui::PushFont(boldFont);
    if (ImGui::Button("Y", buttonSize))
        values.y = resetValue;
    ImGui::PopFont();
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
    ImGui::PopItemWidth();
    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
    ImGui::PushFont(boldFont);
    if (ImGui::Button("Z", buttonSize))
        values.z = resetValue;
    ImGui::PopFont();
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
    ImGui::PopItemWidth();

    ImGui::PopStyleVar();

    ImGui::Columns(1);

    ImGui::PopID();
}


void LeftSidebarWidget::AddComponentDialog(Entity& entity) {
#if defined(_WIN32)
    std::string path = Utils::Window::WindowFileDialog();
#elif defined(__APPLE__) && defined(__MACH__)
    // macOS specific code
#elif defined(__linux__)
    // Linux specific code
#else
    // Unknown or unsupported platform
#endif

    if (!path.empty()) {
        entity.addComponent<ModelComponent>();
        //NOTE: disable for opengl since it doesn't like buffer generation on a separate thread
#define USE_THREAD        
#ifdef USE_THREAD
        AsyncEvent event(path);
        auto func = [&entity](AsyncEvent& event) {
            glfwMakeContextCurrent(AppWindow::sharedWindow);            //EXPERIMENTING SINCE THIS WILL RACE
            if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
                std::cerr << "Failed to initialize GLAD for shared context" << std::endl;
                return;
            }
            ModelComponent& component = entity.getComponent<ModelComponent>();
            component.path = "Loading...";
            SceneManager::getInstance().addModel(event.id.c_str());     // add model to the manager
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
            glfwMakeContextCurrent(AppWindow::window);
        };
        EventManager::getInstance().Queue(event, func);
#else
        ModelLoadEvent event(path, entity);
        EventManager::getInstance().Publish(event);
#endif
        ModelComponent& component = entity.getComponent<ModelComponent>();
        if (component.path == "None") {
            ImGui::OpenPopup("Model loading error");
            errorPopupOpen = true;
        }
    }
}

void LeftSidebarWidget::ErrorModal(const char* message) {
    if (ImGui::BeginPopupModal("Model loading error", &errorPopupOpen, ImGuiWindowFlags_AlwaysAutoResize)){
        ImGui::Text(message);
        ImGui::Separator();

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
        ImGui::PopStyleVar();

        ImGui::SetCursorPosX(ImGui::GetWindowWidth() - ImGui::GetWindowContentRegionMax().x / 2);
        if (ImGui::Button("OK", ImVec2(120, 0))) { 
            ImGui::CloseCurrentPopup(); 
        }
        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) { 
            ImGui::CloseCurrentPopup(); 
        }
        ImGui::EndPopup();
    }
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

void LeftSidebarWidget::AddItemButton(const std::string&& label) {
    if (ImGui::Button(label.c_str(), ImVec2(-1, 0))) {
        SceneManager::getInstance().getScene(ACTIVE_SCENE)->addEntity();
    }
}

void LeftSidebarWidget::LightTab(Entity& entity)
{
    if (entity.hasComponent<MLightComponent>()) {
        MLightComponent& light = entity.getComponent<MLightComponent>();
        TransformComponent& transform = entity.getComponent<TransformComponent>();
        if (ImGui::TreeNodeEx(std::to_string(entity.getID()).c_str())) {
            light.position = transform.translateVec;
            ImGui::DragFloat3("Color", glm::value_ptr(light.color), 0.1f, 1000.0f, 0);
            ImGui::TreePop();
        }
    }
}

void LeftSidebarWidget::EntityTab() {
    //if (ImGui::Begin("Entities")) {
    //    ErrorModel("Unsupported file format, please use an appropriate one");

    //    AddItemButton();

    //    //static ImGuiTextFilter filter;
    //    //filter.Filters;
    //    //filter.Draw("Search");
    //    int currentItem = 0;
    //    static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

    //    //static const char* item_names[] = { "Item One", "Item Two", "Item Three", "Item Four", "Item Five" };
    //    if (SceneManager::components.size() > 0) {
    //        nodes = {};
    //        for (auto& component : SceneManager::components) {
    //            nodes.push_back(component.first);
    //        }
    //    }

    //    size_t prevIndex;
    //    for (int i = 0; i < nodes.size(); i++) {
    //        ImGuiTreeNodeFlags node_flags = base_flags;
    //        prevIndex = selectedIndex;
    //        Component* component = SceneManager::getComponent(nodes[i]);
    //        if (component != nullptr) {
    //            if (selectedIndex == i) {
    //                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.000f, 0.682f, 0.000f, 0.949f));
    //                prevIndex = selectedIndex;
    //                node_flags |= ImGuiTreeNodeFlags_Selected;
    //            }

    //            std::string id_str = "ID: " + component->getID();
    //            std::string selectState = "Selected: " + std::string(component->isSelected() ? "true" : "false");
    //            std::string name = "Name: " + component->getName();
    //            std::string hasAnimation = "Has Animation: " + std::string(component->canAnimate() ? "true" : "false");
    //            static const std::string item_names[] = { id_str, selectState, name, hasAnimation };
    //            if (ImGui::TreeNodeEx(component->getName().c_str(), node_flags))
    //            {
    //                if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
    //                    selectedIndex = i;
    //                selectedIndex == i ? component->select() : component->unSelect();
    //                //if(component->isSelected())
    //                //    node_flags |= ImGuiTreeNodeFlags_Selected;
    //                for (int n = 0; n < IM_ARRAYSIZE(item_names); n++)
    //                {
    //                    const char* item = item_names[n].c_str();
    //                    ImGui::Selectable(item);
    //                }
    //                ImGui::TreePop();
    //            }
    //        }
    //        if (prevIndex == i)
    //            ImGui::PopStyleColor();
    //    }

    //    ImVec2 wsize = ImGui::GetWindowSize();
    //    wsize.x /= 3;
    //    wsize.y /= 3;
    //    //for (auto& [path, model] : SceneManager::getInstance().models) {
    //    //    ImGui::Text(path.c_str());
    //    //    for (auto [path, texture] : model->loaded_textures) {
    //    //        ImGui::Image((ImTextureID)texture.ID, wsize, ImVec2(0, 1), ImVec2(1, 0));
    //    //    }
    //    //}
    //    for (auto& [id, component] : SceneManager::components) {
    //        //ImGui::Text(path.c_str());
    //        //for (auto& mesh : model->meshes) {
    //        //    for (auto texture : mesh.textures) {
    //        //        ImGui::Image(ImTextureID(&texture.ID), wsize, ImVec2(0, 1), ImVec2(1, 0));
    //        //    }
    //        //}
    //        Component* c = component.get();
    //        for (auto [path, texture] : c->model_ptr->loaded_textures) {
    //            //Texture tex(path.c_str(), "diffuse");
    //            ImGui::Text(std::to_string(texture.ID).c_str());
    //            ImGui::Image((ImTextureID)texture.ID, wsize, ImVec2(0, 1), ImVec2(1, 0));
    //        }
    //    }


    //    if (ImGui::Begin("Properties")) {
    //        if (nodes.size() > 0 && selectedIndex <= nodes.size()) {
    //            Component* component = SceneManager::getComponent(nodes[selectedIndex]);
    //            if (component != nullptr) {
    //                SceneManager::setSelectedID(nodes[selectedIndex]);
    //                glm::vec3 translateVector(component->translateVector);
    //                glm::vec3 scaleVector(component->scaleVector);
    //                glm::vec3 rotationVector(component->rotationVector);

    //                ImGui::Text(component->getName().c_str());
    //                if (ImGui::SliderFloat3("Translate", &translateVector[0], -10.0f, 10.0f, 0))
    //                    component->translate(translateVector);
    //                ImGuiIO& io = ImGui::GetIO();
    //                auto boldFont = io.Fonts->Fonts[0];
    //                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
    //                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
    //                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
    //                ImGui::Button("X");
    //                ImGui::PushFont(boldFont);
    //                ImGui::PopFont();
    //                ImGui::PopStyleColor(3);
    //                ImGui::SameLine();
    //                if (ImGui::DragFloat3("Scale", &scaleVector[0], 0.0f, 10.0f, 0))
    //                    component->scale(scaleVector);

    //                ImGui::Text("Material Properties");
    //                ImGui::SliderFloat3("Albedo", glm::value_ptr(component->materialPBR.albedo), 0.0f, 1.0f, 0);
    //                ImGui::SliderFloat("Metalic", &(component->materialPBR.metalic), 0.0f, 1.0f, 0);
    //                ImGui::SliderFloat("Roughness", &(component->materialPBR.roughness), 0.0f, 1.0f, 0);
    //                ImGui::SliderFloat("AO", &component->materialPBR.ao, 0.0f, 1.0f, 0);
    //            }
    //        }
    //    }
    //    ImGui::End();

    //    if (ImGui::Begin("Animation")) {
    //        if (nodes.size() > 0 && selectedIndex <= nodes.size()) {
    //            Component* component = SceneManager::getComponent(nodes[selectedIndex]);
    //            float time;
    //            if (component != nullptr) {
    //                if (!component->canAnimate())
    //                    ImGui::BeginDisabled();
    //                if (ImGui::DragFloat("deltatime", &time))
    //                    component->updateAnimation(time);
    //                if (!component->canAnimate())
    //                    ImGui::EndDisabled();
    //            }
    //        }

    //    }
    //    ImGui::End();
    //}
    //ImGui::End();

    ImGui::End();
    SceneManager& sceneManager = sceneManager.getInstance();
    Scene* scene = sceneManager.getScene(ACTIVE_SCENE);

    if (ImGui::Begin("Scenes")) {
        AddItemButton("+ Add Entity");
        
        Timer("component event", true);

        for (auto& [uuid, entity] : scene->entities) {
            ImGuiTreeNodeFlags node_flags = base_flags;
            ImGui::PushID(std::to_string(uuid).c_str());

            std::string name = entity.getComponent<NameComponent>().name;
            if (entity.hasComponent<ModelComponent>() && name == "Entity") {
                auto& model = entity.getComponent<ModelComponent>().model;
                if (std::shared_ptr modelPtr = model.lock()) {
                    name = modelPtr->getFileName();
                }
            }

            std::string addModelTex = "Add Model Async(broken)";

            if (selectedEntity == &entity) {
                node_flags |= ImGuiTreeNodeFlags_Selected;
            }

            bool open = ImGui::TreeNodeEx(name.c_str(), node_flags);
            bool showPopup = ImGui::BeginPopupContextItem("Add Component");
            bool showTextInput = false;

            if (showTextInput) {
                ImGui::PushID(std::to_string(entity.getID()).c_str());
                NameComponent& nameComponent = entity.getComponent<NameComponent>();
                static char str1[128] = "";
                //ImGui::InputTextWithHint("input text (w/ hint)", "enter text here", str1, IM_ARRAYSIZE(str1));
                ImGui::InputText("Edit Text", str1, sizeof(str1));
                nameComponent.name = str1;

                ImGui::InputText("Edit Text", str1, sizeof(str1));

                // Optionally, add a button to confirm and hide the input field
                if (ImGui::Button("Confirm")) {
                    NameComponent& nameComponent = entity.getComponent<NameComponent>();
                    nameComponent.name = str1;
                    showTextInput = false;
                }

                ImGui::SameLine();

                if (ImGui::Button("Cancel")) {
                    showTextInput = false;
                }
                ImGui::PopID();
            }

            if (showPopup) {
                if (ImGui::MenuItem("Rename")) {
                    showTextInput = true;
                }

                if (ImGui::MenuItem(addModelTex.c_str())) {
                    AddComponentDialog(entity);
                }

                if (ImGui::MenuItem("Load Model blocking")) {
                    //auto& modelComponent = entity.getComponent<ModelComponent>();
                    std::string path = Utils::Window::WindowFileDialog();
                    ModelLoadEvent event(path, entity);
                    EventManager::getInstance().Publish(event);
                }

                if (ImGui::MenuItem("Add Light")) {
                    entity.addComponent<MLightComponent>();
                }

                if (ImGui::MenuItem("Add Animation")) {
                    entity.addComponent<AnimationComponent>();
                }

                if (ImGui::MenuItem("Add Animator")) {
                    entity.addComponent<AnimatorComponent>();
                }

                if (ImGui::MenuItem("Delete Entity")) {
                    scene->removeEntity(uuid);
                }

                ImGui::EndPopup();
            }
            
            if (open) {
                if (entity.hasComponent<ModelComponent>()) {
                    addModelTex = "Change Model";
                    std::string modelPath = "Path: " + entity.getComponent<ModelComponent>().path;
                    ImGui::Text(modelPath.c_str());
                }

                ImGui::Text(std::string("id: " + std::to_string(uuid)).c_str());

                ImGui::SameLine();
                if (ImGui::Button(addModelTex.c_str())) {
                    AddComponentDialog(entity);
                }

                TransformComponent& transform = entity.getComponent<TransformComponent>();
                glm::mat4 matrix = transform.getModelMatrix();
                displayMatrix(matrix);

                if (ImGui::DragFloat3("Position", glm::value_ptr(transform.translateVec), 0.2f, -20.0f, 20.0f)) {
                    transform.updateTransform();
                }

                if (ImGui::DragFloat3("Scale", glm::value_ptr(transform.scaleVec), 0.2f, -20.0f, 20.0f)) {
                    transform.updateTransform();
                }

                if (ImGui::DragFloat3("Rotation", glm::value_ptr(transform.rotateVec), 0.2f, -180.0f, 180.0f)) {
                    transform.updateTransform();
                }

                ImGui::TreePop();
            }


            LightTab(entity);

            if (ImGui::IsItemHovered() && !showPopup) {
                if (ImGui::IsAnyItemHovered()) {
                    ImGui::BeginTooltip();
                    //ImGui::Text(path.c_str());
                    ImGui::EndTooltip();
                }
            }

            // currently support single entity selection
            if (ImGui::IsItemClicked()) {
                selectedEntity = &entity;
                scene->selectEntities({ entity });
            }

            ImGui::PopID();
        }
    }
    ImGui::End();
}

void LeftSidebarWidget::ModelsTab()
{
    SceneManager& sceneManager = sceneManager.getInstance();

    if (ImGui::Begin("Models Browser")) {
        for (auto [path, model] : sceneManager.models) {
            ImGui::PushID(path.c_str());
            ImGuiTreeNodeFlags node_flags = base_flags;

            if (selectedModel == path) {
                node_flags |= ImGuiTreeNodeFlags_Selected;
            }

            bool open = (ImGui::TreeNodeEx(path.c_str(), node_flags));
            bool showPopup = ImGui::BeginPopupContextItem("Add Component");
            if (showPopup) {
                if (ImGui::MenuItem("Copy Path")) {
                    ImGui::SetClipboardText(path.c_str());
                }

                if (ImGui::MenuItem("Load Model")) {
                    std::string path = Utils::Window::WindowFileDialog();
                    if (!path.empty()) {
                        sceneManager.addModel(path);
                    }
                }

                if (ImGui::MenuItem("Delete Model")) {
                    sceneManager.removeModel(path);
                }

                ImGui::EndPopup();
            }

            if (open) {
                ImGui::TreePop();
            }

            if (ImGui::IsItemHovered() && !showPopup) {
                if (ImGui::IsAnyItemHovered()) {
                    ImGui::BeginTooltip();
                    ImGui::Text(path.c_str());
                    ImGui::EndTooltip();
                }
            }

            if (ImGui::IsItemClicked()) {
                selectedModel = path;
            }
        }
        ImGui::PopID();
        ImGui::End();
    }

}



LeftSidebarWidget::LeftSidebarWidget() {
    selectedIndex = 0;
}

void LeftSidebarWidget::render()
{
    ImGui::BeginGroup();
    ErrorModal("Error loading Model");
    EntityTab();
    ModelsTab();
    ImGui::EndGroup();
}
