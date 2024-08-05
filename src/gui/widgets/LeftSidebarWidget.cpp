#include "../headers/LeftSidebarWidget.h"
#include "../../core/scene/SceneManager.h"

#include <windows.h>
#include <shobjidl.h> 
#include "../../graphics/utils/Utils.h"

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

void LeftSidebarWidget::AddEntityButton() {
    if (ImGui::Button("+ Add Entity", ImVec2(-1, 0))) {

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
            std::string id = SceneManager::addComponent(path.c_str());

            if (!id.empty()) {
                size_t pos = path.find_last_of('/');
                nodes.push_back(id);
                selectedIndex = nodes.size() - 1;
                SceneManager::setSelectedID(id);
                Component* component = SceneManager::getComponent(id);
                component->select();
            }
            else {
                ImGui::OpenPopup("Model loading error!");
            }
        }
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
            if (lightComponent!= nullptr && lightComponent->light != nullptr) {
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
    if (ImGui::Begin("Entities")) {
        ErrorModel("Unsupported file format, please use an appropriate one");

        AddEntityButton();

        //static ImGuiTextFilter filter;
        //filter.Filters;
        //filter.Draw("Search");
        int currentItem = 0;
        static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

        //static const char* item_names[] = { "Item One", "Item Two", "Item Three", "Item Four", "Item Five" };
        if (SceneManager::components.size() > 0) {
            nodes = {};
            for (auto& component : SceneManager::components) {
                nodes.push_back(component.first);
            }
        }

        size_t prevIndex;
        for (int i = 0; i < nodes.size(); i++) {
            ImGuiTreeNodeFlags node_flags = base_flags;
            prevIndex = selectedIndex;
            Component* component = SceneManager::getComponent(nodes[i]);
            if (component != nullptr) {
                if (selectedIndex == i) {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.000f, 0.682f, 0.000f, 0.949f));
                    prevIndex = selectedIndex;
                    node_flags |= ImGuiTreeNodeFlags_Selected;
                }

                std::string id_str = "ID: " + component->getID();
                std::string selectState = "Selected: " + std::string(component->isSelected() ? "true" : "false");
                std::string name = "Name: " + component->getName();
                std::string hasAnimation = "Has Animation: " + std::string(component->canAnimate() ? "true" : "false");
                static const std::string item_names[] = { id_str, selectState, name, hasAnimation };
                if (ImGui::TreeNodeEx(component->getName().c_str(), node_flags))
                {
                    if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
                        selectedIndex = i;
                    selectedIndex == i ? component->select() : component->unSelect();
                    //if(component->isSelected())
                    //    node_flags |= ImGuiTreeNodeFlags_Selected;
                    for (int n = 0; n < IM_ARRAYSIZE(item_names); n++)
                    {
                        const char* item = item_names[n].c_str();
                        ImGui::Selectable(item);
                    }
                    ImGui::TreePop();
                }
            }
            if (prevIndex == i)
                ImGui::PopStyleColor();
        }

        if (ImGui::Begin("Properties")) {
            if (nodes.size() > 0 && selectedIndex <= nodes.size()) {
                Component* component = SceneManager::getComponent(nodes[selectedIndex]);
                if (component != nullptr) {
                    SceneManager::setSelectedID(nodes[selectedIndex]);
                    glm::vec3 translateVector(component->translateVector);
                    glm::vec3 scaleVector(component->scaleVector);
                    glm::vec3 rotationVector(component->rotationVector);

                    ImGui::Text(component->getName().c_str());
                    if (ImGui::SliderFloat3("Translate", &translateVector[0], -10.0f, 10.0f, 0))
                        component->translate(translateVector);
                    ImGuiIO& io = ImGui::GetIO();
                    auto boldFont = io.Fonts->Fonts[0];
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
                    ImGui::Button("X");
                    ImGui::PushFont(boldFont);
                    ImGui::PopFont();
                    ImGui::PopStyleColor(3);
                    ImGui::SameLine();
                    if (ImGui::DragFloat3("Scale", &scaleVector[0], 0.0f, 10.0f, 0))
                        component->scale(scaleVector);

                    ImGui::Text("Material Properties");
                    ImGui::SliderFloat3("Albedo", glm::value_ptr(component->materialPBR.albedo), 0.0f, 1.0f, 0);
                    ImGui::SliderFloat("Metalic", &(component->materialPBR.metalic), 0.0f, 1.0f, 0);
                    ImGui::SliderFloat("Roughness", &(component->materialPBR.roughness), 0.0f, 1.0f, 0);
                    ImGui::SliderFloat("AO", &component->materialPBR.ao, 0.0f, 1.0f, 0);
                }
            }
        }
        ImGui::End();

        if (ImGui::Begin("Animation")) {
            if (nodes.size() > 0 && selectedIndex <= nodes.size()) {
                Component* component = SceneManager::getComponent(nodes[selectedIndex]);
                float time;
                if (component != nullptr) {
                    if (!component->canAnimate())
                        ImGui::BeginDisabled();
                    if (ImGui::DragFloat("deltatime", &time))
                        component->updateAnimation(time);
                    if (!component->canAnimate())
                        ImGui::EndDisabled();
                }
            }

        }
        ImGui::End();
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
