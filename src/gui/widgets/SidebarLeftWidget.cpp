#include "../headers/LeftSidebarWidget.h"
#include "../../graphics/GraphicsController.h"

#include <windows.h>
#include <shobjidl.h> 
#include "../../graphics/utils/headers/Utils.h"

LeftSidebarWidget::LeftSidebarWidget() {
    selectedIndex = 0;
}

void LeftSidebarWidget::render()
{
	ImGui::Begin("Entities");

    if (ImGui::BeginPopupModal("Model loading error!", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Unsupported file format");
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
                glm::vec3 translateVector(component->translateVector);
                glm::vec3 scaleVector(component->scaleVector);
                glm::vec3 rotationVector(component->rotationVector);
                glm::vec3 dummy;

                OpenGLController::setSelectedID(nodes[selectedIndex]);
                //std::cout << "current selectedID: " << OpenGLController::getSelectedID() << "\n";


                ImGui::Text(component->getName().c_str());

                if (ImGui::SliderFloat3("Translate", &translateVector[0], -10.0f, 10.0f, 0))
                    component->translate(translateVector);
                if (ImGui::DragFloat3("Scale", &scaleVector[0], 0.0f, 10.0f, 0))
                    component->scale(scaleVector);
                //if (ImGui::SliderFloat3("Rotate", &rotationVector[0], -10.0f, 10.0f, 0))
                //    component->rotate(rotationVector);

                //TODO: place holder for now, add these features later
                ImGui::Text("Material Properties");
                if (ImGui::SliderFloat3("Ambient", &dummy[0], -10.0f, 10.0f, 0))
                    component->scale(scaleVector);
                if (ImGui::SliderFloat3("Diffuse", &dummy[0], -10.0f, 10.0f, 0))
                    component->scale(scaleVector);
                if (ImGui::SliderFloat3("Specular", &dummy[0], -10.0f, 10.0f, 0))
                    component->scale(scaleVector);
                if (ImGui::SliderFloat3("Shininess", &dummy[0], -10.0f, 10.0f, 0))
                    component->scale(scaleVector);
            }
        }
    }
	ImGui::End();

    if (ImGui::Begin("Animation")) {
        Component* component = OpenGLController::getComponent(nodes[selectedIndex]);
        float time;
        if (!component->canAnimate())
            ImGui::BeginDisabled();
        if (ImGui::DragFloat("deltatime", &time)) {
            component->updateAnimation(time);
        }
        if (!component->canAnimate())
            ImGui::EndDisabled();
    }
    ImGui::End();



	ImGui::End();


}
