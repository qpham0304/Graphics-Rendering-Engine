#include "../headers/LeftSidebarWidget.h"
#include "../../graphics/GraphicsController.h"

#include <windows.h>
#include <shobjidl.h> 

std::string WindowFileDialog(HWND hwnd) {
    std::string filePath;

    IFileOpenDialog* pFileOpen = NULL;
    HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));
    if (SUCCEEDED(hr)) {
        // Set options for the file dialog
        DWORD dwFlags;
        hr = pFileOpen->GetOptions(&dwFlags);
        if (SUCCEEDED(hr)) {
            hr = pFileOpen->SetOptions(dwFlags | FOS_FORCEFILESYSTEM);
        }

        // Show the file dialog
        if (SUCCEEDED(hr)) {
            hr = pFileOpen->Show(hwnd);
        }

        // Get the selected file path
        if (SUCCEEDED(hr)) {
            IShellItem* pItem = NULL;
            hr = pFileOpen->GetResult(&pItem);
            if (SUCCEEDED(hr)) {
                PWSTR pszFilePath = NULL;
                hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
                if (SUCCEEDED(hr)) {
                    // Convert wchar_t* to std::string
                    std::wstring ws(pszFilePath);
                    filePath = std::string(ws.begin(), ws.end());
                    CoTaskMemFree(pszFilePath);
                }
                pItem->Release();
            }
        }
        pFileOpen->Release();
    }

    return filePath;
}

LeftSidebarWidget::LeftSidebarWidget() {
    selectedIndex = 0;
}

void LeftSidebarWidget::render()
{
	ImGui::Begin("Entities");

    if (ImGui::Button("+ Add Object", ImVec2(-1, 0))) {
        
#if defined(_WIN32)
        HWND hwnd = ::GetActiveWindow();
        std::string path = WindowFileDialog(hwnd);
        for (char& c : path) {
            if (c == '\\') {
                c = '/';
            }
        }
#elif defined(__APPLE__) && defined(__MACH__)
        // macOS specific code
#elif defined(__linux__)
        // Linux specific code
#else
        // Unknown or unsupported platform
#endif
        if (!path.empty()) {
            std::string id = OpenGLController::addComponent(path.c_str());
            size_t pos = path.find_last_of('/');
            nodes.push_back(id);
            selectedIndex = nodes.size() - 1;
            OpenGLController::setSelectedID(id);
            Component* component = OpenGLController::getComponent(id);    // PLS DONT FREE THIS
            component->select();
        }
    }

    //static ImGuiTextFilter filter;
    //filter.Filters;
    //filter.Draw("Search");

    bool flag = ImGuiTreeNodeFlags_OpenOnArrow;
    int prevIndex;
    for (int i = 0; i < nodes.size(); i++) {
        prevIndex = selectedIndex;
        Component* component = OpenGLController::getComponent(nodes[i]);    // PLS DONT FREE THIS
        if (selectedIndex == i) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 1.0f, 1.0f));
            prevIndex = selectedIndex;
        }
        else {
            ImGui::SetNextItemOpen(false);
        }
        if (ImGui::TreeNodeEx(component->getName().c_str(), ImGuiTreeNodeFlags_OpenOnArrow))
        {
            if (ImGui::IsItemClicked())
            {
                selectedIndex = i;
            }
            selectedIndex == i ? component->select() : component->unSelect();


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
            ImGui::TreePop();
        }
        if (prevIndex == i)
            ImGui::PopStyleColor();
    }

    if (ImGui::Begin("Properties")) {
        if (nodes.size() > 0 && selectedIndex <= nodes.size()) {
            Component* component = OpenGLController::getComponent(nodes[selectedIndex]);    // PLS DONT FREE THIS
            
            glm::vec3 translateVector(component->translateVector);
            glm::vec3 scaleVector(component->scaleVector);
            glm::vec3 rotationVector(component->rotationVector);
            glm::vec3 dummy;
            
            OpenGLController::setSelectedID(nodes[selectedIndex]);
            ImGui::Text(component->getName().c_str());
            if (ImGui::SliderFloat3("Translate", &translateVector[0], -10.0f, 10.0f, 0))
                component->translate(translateVector);
            if (ImGui::SliderFloat3("Scale", &scaleVector[0], 0.0f, 10.0f, 0))
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
	    ImGui::End();
    }


    ImGui::Begin("Animation");



    ImGui::End();

	ImGui::End();
}
