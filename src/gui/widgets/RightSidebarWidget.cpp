#include "../headers/RightSidebarWidget.h"
#include "../../core/scene/SceneManager.h"
#include "../../core/layers/LayerManager.h"
#include "../../core/components/MComponent.h"

RightSidebarWidget::RightSidebarWidget()
{
    popupOpen = false;
}

void RightSidebarWidget::TextureModal(const ImTextureID& id) {
    //ImVec2 appSize = ImGui::GetIO().DisplaySize;
    //ImVec2 popupSize = ImVec2(appSize.y * 0.75f, appSize.y * 0.75f);
    //ImGui::SetNextWindowSize(popupSize);
    
    ImGuiStyle& style = ImGui::GetStyle();
    style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.0, 0.0, 0.0, 0.5);
    if (ImGui::BeginPopupModal("Image View", &popupOpen, ImGuiWindowFlags_AlwaysAutoResize)) {
        //ImVec2 availableSize = ImGui::GetContentRegionAvail();
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImVec2 availableSize = ImVec2(viewport->Size.y * 0.75, viewport->Size.y * 0.75);
        float aspectRatio = 1.0 / 1.0;

        ImVec2 displaySize;
        if (availableSize.x / availableSize.y > aspectRatio) {
            displaySize.y = availableSize.y;
            displaySize.x = availableSize.y * aspectRatio;
        }
        else {
            displaySize.x = availableSize.x;
            displaySize.y = availableSize.x / aspectRatio;
        }

        ImGui::Image(id, displaySize);

        ImGui::Separator();
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
        ImGui::PopStyleVar();

        if (ImGui::IsMouseClicked(0) && !ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow)) {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void RightSidebarWidget::layersControl()
{
    ImGui::Begin("Layers");

    ImGui::End();
}

void RightSidebarWidget::textureView()
{
    ImGui::Begin("Texture View");
    TextureModal((ImTextureID)selectedTexture);

    Scene* scene = SceneManager::getInstance().getActiveScene();
    auto selectedEntities = scene->getSelectedEntities();
    for (auto& entity : selectedEntities) {
        ImVec2 wsize = ImGui::GetWindowSize();
        wsize.x /= 5;
        wsize.y = wsize.x;
        if (entity.hasComponent<ModelComponent>()) {
            auto& modelComponent = entity.getComponent<ModelComponent>();
            std::shared_ptr modelPtr = modelComponent.model.lock();
            if (modelPtr) {
                for (auto [path, texture] : modelPtr->loaded_textures) {
                    ImGui::PushID(path.c_str());
                    ImGui::Separator();
                    ImGui::Image((ImTextureID)texture.ID, wsize, ImVec2(0, 1), ImVec2(1, 0));
                    ImGui::PopID();
                    
                    if (ImGui::IsItemClicked()) {
                        selectedTexture = texture.ID;
                        ImGui::OpenPopup("Image View");
                        popupOpen = true;
                    }
                    
                    ImGui::SameLine();
                    ImGui::TextWrapped(path.c_str());
                }
            }
        }
    }
    ImGui::End();
}

void RightSidebarWidget::environmentControl()
{
    ImGui::Begin("Environment Control");

    ImGui::End();
}

void RightSidebarWidget::render()
{
    ImGui::BeginGroup();
    //if (ImGui::Begin("Tab bar")) {
    //    ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
    //    if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
    //    {
    //        if (ImGui::BeginTabItem("Avocado"))
    //        {
    //            layersControl();
    //            ImGui::EndTabItem();
    //        }
    //        if (ImGui::BeginTabItem("Broccoli"))
    //        {
    //            textureView();
    //            ImGui::EndTabItem();
    //        }
    //        if (ImGui::BeginTabItem("Cucumber"))
    //        {
    //            environmentControl();
    //            ImGui::EndTabItem();
    //        }
    //        ImGui::EndTabBar();
    //    }
    //    ImGui::End();
    //}
    layersControl();
    textureView();
    environmentControl();
    ImGui::EndGroup();

}
