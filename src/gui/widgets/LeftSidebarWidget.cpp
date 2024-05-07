#include "../headers/LeftSidebarWidget.h"
#include "../../graphics/GraphicsController.h"

#include <windows.h>
#include <shobjidl.h> 
#include "../../graphics/utils/headers/Utils.h"

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


LeftSidebarWidget::LeftSidebarWidget() {
    selectedIndex = 0;
}

void LeftSidebarWidget::render()
{
	ImGui::Begin("Entities");

    if (ImGui::BeginPopupModal("Model loading error!", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Unsupported file format, please use an appropriate one");
        ImGui::Separator();

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
        ImGui::PopStyleVar();

        if (ImGui::Button("OK", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
        ImGui::EndPopup();
    }

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
            std::string id = OpenGLController::addComponent(path.c_str());

            if (!id.empty()) {
                size_t pos = path.find_last_of('/');
                nodes.push_back(id);
                selectedIndex = nodes.size() - 1;
                OpenGLController::setSelectedID(id);
                Component* component = OpenGLController::getComponent(id);
                component->select();
            }
            else {
                ImGui::OpenPopup("Model loading error!");
            }
        }
    }

    //static ImGuiTextFilter filter;
    //filter.Filters;
    //filter.Draw("Search");
    int currentItem = 0;
    static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

    //static const char* item_names[] = { "Item One", "Item Two", "Item Three", "Item Four", "Item Five" };
    if (OpenGLController::components.size() > 0) {
        nodes = {};
        for (auto& component : OpenGLController::components) {
            nodes.push_back(component.first);
        }
    }

    size_t prevIndex;
    for (int i = 0; i < nodes.size(); i++) {
        ImGuiTreeNodeFlags node_flags = base_flags;
        prevIndex = selectedIndex;
        Component* component = OpenGLController::getComponent(nodes[i]);
        if (component != nullptr) {
            if (selectedIndex == i) {
                //ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.000f, 0.682f, 0.000f, 0.949f));
                prevIndex = selectedIndex;
                node_flags |= ImGuiTreeNodeFlags_Selected;
            }
            if (ImGui::TreeNodeEx(component->getName().c_str(), node_flags))
            {
                if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
                    selectedIndex = i;
                selectedIndex == i ? component->select() : component->unSelect();
                //if(component->isSelected())
                //    node_flags |= ImGuiTreeNodeFlags_Selected;

                static const char* item_names[] = { "Item One", "Item Two", "Item Three", "Item Four", "Item Five" };
                for (int n = 0; n < IM_ARRAYSIZE(item_names); n++)
                {
                    const char* item = item_names[n];
                    ImGui::Selectable(item);

                    //if (ImGui::IsItemActive())
                    //{
                    //    int n_next = n + (ImGui::GetMouseDragDelta(0).y < 0.f ? -1 : 1);
                    //    if (n_next >= 0 && n_next < IM_ARRAYSIZE(item_names))
                    //    {
                    //        item_names[n] = item_names[n_next];
                    //        item_names[n_next] = item;
                    //        ImGui::ResetMouseDragDelta();
                    //    }
                    //}
                }
                ImGui::TreePop();
            }
        }
        
        //if (prevIndex == i)
        //    ImGui::PopStyleColor();
    }

    if (ImGui::Begin("Properties")) {
        if (nodes.size() > 0 && selectedIndex <= nodes.size()) {
            Component* component = OpenGLController::getComponent(nodes[selectedIndex]);
            if (component != nullptr) {
                OpenGLController::setSelectedID(nodes[selectedIndex]);
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
                //if (ImGui::SliderFloat3("Rotate", &rotationVector[0], -10.0f, 10.0f, 0))
                //    component->rotate(rotationVector);

                //DrawVec3Control("Translation", translateVector);
                //glm::vec3 rotation = glm::degrees(component->rotationVector);
                //DrawVec3Control("Rotation", rotation);
                //component->rotationVector = glm::radians(rotation);
                //DrawVec3Control("Scale", component->scaleVector, 1.0f);

                ImGui::Text("Material Properties");
                //ImGui::SliderFloat3("Ambient", glm::value_ptr(component->material.ambient), 0.0f, 1.0f, 0);
                //ImGui::SliderFloat3("Diffuse", glm::value_ptr(component->material.diffuse), 0.0f, 1.0f, 0);
                ImGui::SliderFloat3("Specular", glm::value_ptr(component->material.specular), -1.0, 1.0f, 0);
                ImGui::SliderFloat("Shininess", &component->material.shininess, 8.0f, 128.0f, 0);
            }
        }
    }
	ImGui::End();

    if (ImGui::Begin("Animation")) {
        if (nodes.size() > 0 && selectedIndex <= nodes.size()) {
            Component* component = OpenGLController::getComponent(nodes[selectedIndex]);
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



	ImGui::End();


}
