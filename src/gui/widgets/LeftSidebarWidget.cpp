#include "../headers/LeftSidebarWidget.h"
#include "../../core/scene/SceneManager.h"
#include "../../events/EventManager.h"

#include <windows.h>
#include <shobjidl.h> 
#include "../../graphics/utils/Utils.h"
#include "../../core/components/MComponent.h"
#include "Texture.h"
#include "../../core/scene/SceneManager.h"

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


static void ErrorModel(const char* message) {
    if (ImGui::BeginPopupModal("Model loading error!", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text(message);
        ImGui::Separator();

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
        ImGui::PopStyleVar();

        ImGui::SetCursorPosX(ImGui::GetWindowWidth() - ImGui::GetWindowContentRegionMax().x / 2);
        if (ImGui::Button("OK", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
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

//if (ImGui::Button("+ Add Entity", ImVec2(-1, 0))) {
//    std::string id = SceneManager::getInstance().getScene(ACTIVE_SCENE)->addEntity();

void LeftSidebarWidget::AddEntityButton() {
    if (ImGui::Button("+ Add Entity", ImVec2(-1, 0))) {
        std::string id = SceneManager::getInstance().getScene(ACTIVE_SCENE)->addEntity();
    }
}

void LeftSidebarWidget::LightTab()
{
    if (ImGui::Begin("Lights")) {
        if (ImGui::Button("+ Add Light", ImVec2(-1, 0))) {
            if (SceneManager::lights.size() < 4) {
                SceneManager::addPointLight(lightPositions[0], lightColors[0]);
                SceneManager::addPointLight(lightPositions[1], lightColors[1]);
                SceneManager::addPointLight(lightPositions[2], lightColors[2]);
                SceneManager::addPointLight(lightPositions[3], lightColors[3]);
            }
            else
                std::cout << "maximum number of lights reached" << std::endl;
        }
        for (auto& pair : SceneManager::lights) {
            std::string name = pair.first;
            auto lightComponent = pair.second.get();
            if (lightComponent != nullptr && lightComponent->light != nullptr) {
                Light* light = lightComponent->light.get();
                if (ImGui::TreeNodeEx(name.c_str())) {
                    ImGui::SliderFloat3("Position", glm::value_ptr(light->position), -20.0f, 20.0f, 0);
                    ImGui::SliderFloat3("Color", glm::value_ptr(light->color), 0.0f, 300.0f, 0);
                    ImGui::TreePop();
                }
            }
        }
    }
    ImGui::End();
}

void LeftSidebarWidget::EntityTab() {
    //if (ImGui::Begin("Entities")) {
    //    ErrorModel("Unsupported file format, please use an appropriate one");

    //    AddEntityButton();

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


    SceneManager& sceneManager = sceneManager.getInstance();
    if (ImGui::Begin("Scenes")) {
        AddEntityButton();
        Scene* scene = sceneManager.getScene(ACTIVE_SCENE);
        Timer("component event", true);

        //if(ImGui::TreeNodeEx(scene->getName().c_str())) {
        //	for (auto& layer : scene->layerManager) {
        //		ImGui::TreeNodeEx(layer->GetName().c_str());
        //	}
        //}

        for (auto& [uuid, entity] : scene->entities) {
            //ImGui::SameLine();
            //if (ImGui::Button("Add Animation")) {
            //    entity.addComponent<AnimationComponent>();
            //}
            //ImGui::SameLine();
            //if (ImGui::Button("Add Animator")) {
            //	entity.addComponent<MeshComponent>();
            //}
            if (ImGui::TreeNodeEx(uuid.c_str())) {
                if (ImGui::Button("remove entity")) {
                    scene->removeEntity(uuid);
                }

                ImGui::SameLine();

                if (ImGui::Button("Add Model")) {
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
//#define USE_THREAD        
#ifdef USE_THREAD
                        AsyncEvent event(path);
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
                        EventManager::getInstance().Queue(event, func);
#else
                        ModelLoadEvent event(path, entity);
                        EventManager::getInstance().Publish(event);
                        ModelComponent& component = entity.getComponent<ModelComponent>();
                        if (component.path == "None") {
                            ImGui::OpenPopup("Failed to load file, please check the format");
                            ErrorModel("Failed to load file, please check the format");
#endif
                        }
                    }
                }
                TransformComponent& transform = entity.getComponent<TransformComponent>();
                displayMatrix(transform.model);

                ImGui::SliderFloat3("Position", glm::value_ptr(translateVec), -20.0f, 20.0f, 0);
                transform.model = glm::translate(glm::mat4(1.0), translateVec);

                ImGui::Text(entity.getComponent<NameComponent>().name.c_str());
                if (entity.hasComponent<ModelComponent>()) {
                    ImGui::Text(entity.getComponent<ModelComponent>().path.c_str());
                }
            }
        }
    }
    ImGui::End();

}

LeftSidebarWidget::LeftSidebarWidget() {
    selectedIndex = 0;
}

void LeftSidebarWidget::render()
{
    LightTab();
    EntityTab();
}
