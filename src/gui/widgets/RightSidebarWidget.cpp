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
        ImVec2 availableSize = ImGui::GetContentRegionAvail();
        float aspectRatio = 1.0;

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

void RightSidebarWidget::layersControl()
{
    ImGui::Begin("Layers");

    ImGui::End();
}

void RightSidebarWidget::textureView()
{
    ImGui::Begin("Texture View");
    TextureModal((ImTextureID)selectedTexture);

    Scene* scene = SceneManager::getInstance().getScene(ACTIVE_SCENE);
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
                    /*ImGui::ImageButton(path.c_str(), (ImTextureID)texture.ID, wsize, ImVec2(0, 1), ImVec2(1, 0));*/
                    //ImVec2 size = wsize;
                    //ImVec2 uv0 = ImVec2(0.0f, 1.0f);                           // UV coordinates
                    //ImVec2 uv1 = ImVec2(1.0f, 0.0f);                           // UV coordinates
                    //ImVec4 bg_col = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);           
                    //ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);         
                    //ImGui::ImageButton("Button Label", (ImTextureID)texture.ID, size, uv0, uv1, bg_col, tint_col);
                    ImGui::Separator();
                    ImGui::Image((ImTextureID)texture.ID, wsize, ImVec2(0, 1), ImVec2(1, 0));
                    ImGui::SameLine();
                    ImGui::TextWrapped(path.c_str());
                    ImGui::PopID();
                    if (ImGui::IsItemClicked()) {
                        selectedTexture = texture.ID;
                        ImGui::OpenPopup("Image View");
                        popupOpen = true;
                    }
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
