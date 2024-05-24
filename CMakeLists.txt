cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

set(CMAKE_SYSTEM_VERSION 10.0 CACHE STRING "" FORCE)

project(comprehensive C CXX)

################################################################################
# Set target arch type if empty. Visual studio solution generator provides it.
################################################################################
if(NOT CMAKE_VS_PLATFORM_NAME)
    set(CMAKE_VS_PLATFORM_NAME "x64")
endif()
message("${CMAKE_VS_PLATFORM_NAME} architecture in use")

if(NOT ("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64"
     OR "${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86"))
    message(FATAL_ERROR "${CMAKE_VS_PLATFORM_NAME} arch is not supported!")
endif()

################################################################################
# Global configuration types
################################################################################
set(CMAKE_CONFIGURATION_TYPES
    "Debug"
    "Release"
    CACHE STRING "" FORCE
)

################################################################################
# Global compiler options
################################################################################
if(MSVC)
    # remove default flags provided with CMake for MSVC
    set(CMAKE_C_FLAGS "")
    set(CMAKE_C_FLAGS_DEBUG "")
    set(CMAKE_C_FLAGS_RELEASE "")
    set(CMAKE_CXX_FLAGS "")
    set(CMAKE_CXX_FLAGS_DEBUG "")
    set(CMAKE_CXX_FLAGS_RELEASE "")
endif()

################################################################################
# Global linker options
################################################################################
if(MSVC)
    # remove default flags provided with CMake for MSVC
    set(CMAKE_EXE_LINKER_FLAGS "")
    set(CMAKE_MODULE_LINKER_FLAGS "")
    set(CMAKE_SHARED_LINKER_FLAGS "")
    set(CMAKE_STATIC_LINKER_FLAGS "")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS}")
    set(CMAKE_MODULE_LINKER_FLAGS_DEBUG "${CMAKE_MODULE_LINKER_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_STATIC_LINKER_FLAGS_DEBUG "${CMAKE_STATIC_LINKER_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS}")
    set(CMAKE_MODULE_LINKER_FLAGS_RELEASE "${CMAKE_MODULE_LINKER_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_STATIC_LINKER_FLAGS_RELEASE "${CMAKE_STATIC_LINKER_FLAGS}")
endif()

################################################################################
# Nuget packages function stub.
################################################################################
function(use_package TARGET PACKAGE VERSION)
    message(WARNING "No implementation of use_package. Create yours. "
                    "Package \"${PACKAGE}\" with version \"${VERSION}\" "
                    "for target \"${TARGET}\" is ignored!")
endfunction()

################################################################################
# Common utils
################################################################################
include(CMake/Utils.cmake)

################################################################################
# Additional Global Settings(add specific info there)
################################################################################
include(CMake/GlobalSettingsInclude.cmake OPTIONAL)

################################################################################
# Use solution folders feature
################################################################################
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

################################################################################
# Sub-projects
################################################################################



























set(PROJECT_NAME OpenGLGameEngine)

################################################################################
# Source groups
################################################################################
set(no_group_source_files
    "Shaders/areaLight.frag"
    "Shaders/areaLight.vert"
    "Shaders/background.frag"
    "Shaders/background.vert"
    "Shaders/brdf.frag"
    "Shaders/brdf.vert"
    "Shaders/cellShader.frag"
    "Shaders/cellShader.vert"
    "Shaders/cubemap-hdr.vert"
    "Shaders/cubemap.frag"
    "Shaders/cubemap.vert"
    "Shaders/default-2.frag"
    "Shaders/default-2.vert"
    "Shaders/default.frag"
    "Shaders/default.geom"
    "Shaders/default.vert"
    "Shaders/equireRectToCubemap.frag"
    "Shaders/irradianceConvolution.frag"
    "Shaders/light.frag"
    "Shaders/light.vert"
    "Shaders/material.frag"
    "Shaders/material.vert"
    "Shaders/model.frag"
    "Shaders/model.vert"
    "Shaders/prefilter.frag"
    "Shaders/shadow/pointShadowsDepth.frag"
    "Shaders/shadow/pointShadowsDepth.geom"
    "Shaders/shadow/pointShadowsDepth.vert"
    "Shaders/shadowMap.frag"
    "Shaders/shadowMap.vert"
    "Shaders/skeletonModel.frag"
    "Shaders/skeletonModel.vert"
    "Shaders/skybox.frag"
    "Shaders/skybox.vert"
)
source_group("" FILES ${no_group_source_files})

set(Header_Files
    "Headers/OpenGL/Animation.h"
    "Headers/OpenGL/Animator.h"
    "Headers/OpenGL/assimp_glm_helpers.h"
    "Headers/OpenGL/Axis.h"
    "Headers/OpenGL/Bone.h"
    "Headers/OpenGL/camera.h"
    "Headers/OpenGL/DepthCubeMap.h"
    "Headers/OpenGL/DepthMap.h"
    "Headers/OpenGL/EBO.h"
    "Headers/OpenGL/FrameBuffer.h"
    "Headers/OpenGL/Mesh.h"
    "Headers/OpenGL/model.h"
    "Headers/OpenGL/resource.h"
    "Headers/OpenGL/Shader.h"
    "Headers/OpenGL/skybox.h"
    "Headers/OpenGL/Texture.h"
    "Headers/OpenGL/VAO.h"
    "Headers/OpenGL/VBO.h"
    "Libraries/include/imgui/IconsFontAwesome5.h"
    "Libraries/include/ImGuizmo/GraphEditor.h"
    "Libraries/include/ImGuizmo/ImCurveEdit.h"
    "Libraries/include/ImGuizmo/ImGradient.h"
    "Libraries/include/ImGuizmo/ImGuizmo.h"
    "Libraries/include/ImGuizmo/ImSequencer.h"
    "Libraries/include/ImGuizmo/ImZoomSlider.h"
    "src/apps/area-light/area_light_demo.h"
    "src/apps/image-based-rendering/pbr_demo.h"
    "src/graphics/components/headers/Component.h"
    "src/graphics/components/headers/GridComponent.h"
    "src/graphics/components/headers/LightComponent.h"
    "src/graphics/components/headers/SkyboxComponent.h"
    "src/graphics/GraphicsController.h"
    "src/graphics/utils/headers/ltc_matrix.h"
    "src/graphics/utils/headers/Utils.h"
    "src/gui/GuiController.h"
    "src/gui/headers/ConsoleLogWidget.h"
    "src/gui/headers/EntityControlWidget.h"
    "src/gui/headers/LeftSidebarWidget.h"
    "src/gui/headers/MainViewWidget.h"
    "src/gui/headers/MenuWidget.h"
    "src/gui/headers/RightSidebarWidget.h"
    "src/gui/headers/SubViewWidget.h"
    "src/gui/headers/Themes.h"
    "src/gui/headers/Widget.h"
    "src/SceneRenderer.h"
)
source_group("Header Files" FILES ${Header_Files})

set(Source_Files
    "glad.c"
    "Libraries/include/ImGuizmo/GraphEditor.cpp"
    "Libraries/include/ImGuizmo/ImCurveEdit.cpp"
    "Libraries/include/ImGuizmo/ImGradient.cpp"
    "Libraries/include/ImGuizmo/ImGuizmo.cpp"
    "Libraries/include/ImGuizmo/ImSequencer.cpp"
    "src/apps/area-light/area_light_demo.cpp"
    "src/apps/image-based-rendering/pbr_demo.cpp"
    "src/apps/sanbox/main.cpp"
    "src/graphics/components/Component.cpp"
    "src/graphics/components/LightComponent.cpp"
    "src/graphics/components/SkyboxComponent.cpp"
    "src/graphics/core/OpenGL/Animation.cpp"
    "src/graphics/core/OpenGL/Animator.cpp"
    "src/graphics/core/OpenGL/Bone.cpp"
    "src/graphics/core/OpenGL/camera.cpp"
    "src/graphics/core/OpenGL/DepthCubeMap.cpp"
    "src/graphics/core/OpenGL/DepthMap.cpp"
    "src/graphics/core/OpenGL/EBO.cpp"
    "src/graphics/core/OpenGL/FrameBuffer.cpp"
    "src/graphics/core/OpenGL/Mesh.cpp"
    "src/graphics/core/OpenGL/Model.cpp"
    "src/graphics/core/OpenGL/Shader.cpp"
    "src/graphics/core/OpenGL/skybox.cpp"
    "src/graphics/core/OpenGL/stb.cpp"
    "src/graphics/core/OpenGL/Texture.cpp"
    "src/graphics/core/OpenGL/VAO.cpp"
    "src/graphics/core/OpenGL/VBO.cpp"
    "src/graphics/GraphicsController.cpp"
    "src/graphics/utils/Utils.cpp"
    "src/gui/GuiController.cpp"
    "src/gui/widgets/ConsoleLogWidget.cpp"
    "src/gui/widgets/EntityControlWidget.cpp"
    "src/gui/widgets/LeftSidebarWidget.cpp"
    "src/gui/widgets/MainViewWidget.cpp"
    "src/gui/widgets/MenuWidget.cpp"
    "src/gui/widgets/RightSidebarWidget.cpp"
    "src/gui/widgets/SubViewWidget.cpp"
    "src/gui/widgets/Widget.cpp"
    "src/SceneRenderer.cpp"
)
source_group("Source Files" FILES ${Source_Files})

set(externals
    "Libraries/include/imgui/backends/imgui_impl_glfw.cpp"
    "Libraries/include/imgui/backends/imgui_impl_opengl2.cpp"
    "Libraries/include/imgui/backends/imgui_impl_opengl3.cpp"
    "Libraries/include/imgui/imgui.cpp"
    "Libraries/include/imgui/imgui_demo.cpp"
    "Libraries/include/imgui/imgui_draw.cpp"
    "Libraries/include/imgui/imgui_tables.cpp"
    "Libraries/include/imgui/imgui_widgets.cpp"
)
source_group("externals" FILES ${externals})

set(ALL_FILES
    ${no_group_source_files}
    ${Header_Files}
    ${Source_Files}
    ${externals}
)

################################################################################
# Target
################################################################################
add_executable(${PROJECT_NAME} ${ALL_FILES})

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE comprehensive)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_GLOBAL_KEYWORD "Win32Proj"
)
################################################################################
# Output directory
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_DEBUG   "${CMAKE_SOURCE_DIR}/bin/${PROJECT_NAME}/$<CONFIG>-${CMAKE_VS_PLATFORM_NAME}"
        OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/bin/${PROJECT_NAME}/$<CONFIG>-${CMAKE_VS_PLATFORM_NAME}"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_DEBUG   "${CMAKE_SOURCE_DIR}/bin/${PROJECT_NAME}/$<CONFIG>-${CMAKE_VS_PLATFORM_NAME}"
        OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/bin/${PROJECT_NAME}/$<CONFIG>-${CMAKE_VS_PLATFORM_NAME}"
    )
endif()
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
    )
endif()
################################################################################
# Compile definitions
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Release>:"
            "NDEBUG"
        ">"
        "_CONSOLE;"
        "UNICODE;"
        "_UNICODE"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Release>:"
            "NDEBUG"
        ">"
        "WIN32;"
        "_CONSOLE;"
        "UNICODE;"
        "_UNICODE"
    )
endif()

################################################################################
# Compile and link options
################################################################################
if(MSVC)
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:
                /Oi;
                /Gy
            >
            /permissive-;
            /std:c++17;
            /sdl;
            /W3;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:
                /Oi;
                /Gy
            >
            /permissive-;
            /std:c++17;
            /sdl;
            /W3;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
    endif()
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:
                /OPT:REF;
                /OPT:ICF
            >
            /DEBUG;
            /SUBSYSTEM:CONSOLE
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:
                /OPT:REF;
                /OPT:ICF
            >
            /DEBUG;
            /SUBSYSTEM:CONSOLE
        )
    endif()
endif()

################################################################################
# Dependencies
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "glfw3;"
        "opengl32;"
        "assimp-vc143-mtd"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "glfw3;"
        "opengl32;"
        "assimp-vc143-mtd"
    )
endif()
target_link_libraries(${PROJECT_NAME} PRIVATE "${ADDITIONAL_LIBRARY_DEPENDENCIES}")

