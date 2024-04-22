#include "../headers/LeftSidebarWidget.h"
#include "../../graphics/GraphicsController.h"

LeftSidebarWidget::LeftSidebarWidget() = default;

void LeftSidebarWidget::render()
{
	ImGui::Begin("Entities");

    if (ImGui::Button("+ Add Object", ImVec2(-1, 0))) {
        
        std::string id = OpenGLController::addComponent("Models/reimu/reimu.obj");
        std::cout << id << "\n";
        //nodes.push_back("entity no." + std::to_string(nodes.size()));
        //
        //std::string str = "entity " + id;
        //entities_map[id] = str;
    }

    //static const char* tmp[] = { "Item One", "Item Two", "Item Three", "Item Four", "Item Five" };
    //static ImGuiTextFilter filter;
    //filter.Filters;
    //filter.Draw("Search");


    for (int i = 0; i < nodes.size(); i++) {
        if (ImGui::TreeNode(nodes[i].c_str()))
        {
            static const char* item_names[] = { "Item One", "Item Two", "Item Three", "Item Four", "Item Five" };
            for (int n = 0; n < IM_ARRAYSIZE(item_names); n++)
            {
                const char* item = item_names[n];
                ImGui::Selectable(item);

                if (ImGui::IsItemActive() && !ImGui::IsItemHovered())
                {
                    int n_next = n + (ImGui::GetMouseDragDelta(0).y < 0.f ? -1 : 1);
                    if (n_next >= 0 && n_next < IM_ARRAYSIZE(item_names))
                    {
                        item_names[n] = item_names[n_next];
                        item_names[n_next] = item;
                        ImGui::ResetMouseDragDelta();
                    }
                }
            }

            //const char* item = nodes[i].c_str();
            //ImGui::Selectable(item);
            //if (ImGui::IsItemActive() && !ImGui::IsItemHovered())
            //{
            //    int n_next = i + (ImGui::GetMouseDragDelta(0).y < 0.f ? -1 : 1);
            //    if (n_next >= 0 && n_next < nodes.size())
            //    {
            //        nodes[i] = nodes[n_next];
            //        nodes[n_next] = item;
            //        ImGui::ResetMouseDragDelta();
            //    }
            //}
            ImGui::TreePop();
        }
    }

	ImGui::Begin("Properties");
	ImGui::Text("child 2 content");
	ImGui::End();

	ImGui::End();
}
