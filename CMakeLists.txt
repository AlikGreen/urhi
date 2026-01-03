cmake_minimum_required(VERSION 3.29)
project(neonRHI)

set(CMAKE_CXX_STANDARD 23)


set(ENABLE_OPT OFF CACHE BOOL "" FORCE)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glslang)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glm)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/dependencies/spirv-cross)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glfw)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glad)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/dependencies/neonLog)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/dependencies/neonCore)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/dependencies/rapidyaml)

### NEON RHI

file(GLOB_RECURSE
        SRC_FILES
        CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
)

add_library(neonRHI STATIC ${SRC_FILES})

target_compile_definitions(neonRHI PRIVATE GLFW_INCLUDE_NONE)

target_include_directories(neonRHI PUBLIC src/)
target_include_directories(neonRHI PUBLIC include/)
target_link_libraries(neonRHI PUBLIC glfw glm::glm glslang spirv-cross-c glad neonLog neonCore)


### IMGUI LIB
set(IMGUI_SOURCES
        dependencies/imgui/imgui.cpp
        dependencies/imgui/imgui_demo.cpp
        dependencies/imgui/imgui_draw.cpp
        dependencies/imgui/imgui_tables.cpp
        dependencies/imgui/imgui_widgets.cpp
        dependencies/imgui/misc/cpp/imgui_stdlib.cpp
)

# add all ImGuizmo sources
file(GLOB IMGUZMO_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/imguizmo/*.cpp"
)
list(APPEND IMGUI_SOURCES ${IMGUZMO_SOURCES})

add_library(imgui_static STATIC ${IMGUI_SOURCES})

target_include_directories(imgui_static PUBLIC
        dependencies/imgui
        dependencies/imguizmo
)

### IMGUI BACKEND
add_library(neonRHI_imgui STATIC
        backends/imgui/imGuiController.cpp
        backends/imgui/imGuiController.h
        backends/imgui/imGuiConfig.h
        backends/imgui/imGuiExtensions.h
        backends/imgui/imGuiExtensions.cpp)

target_include_directories(neonRHI_imgui PUBLIC backends/)
target_link_libraries(neonRHI_imgui PUBLIC neonRHI imgui_static neonCore ryml::ryml c4core::c4core)

target_compile_definitions(neonRHI_imgui PRIVATE
        IMGUI_USER_CONFIG="imgui/imGuiConfig.h"
        IMGUI_DISABLE_OBSOLETE_FUNCTIONS
)

add_library(neonRHI::imgui ALIAS neonRHI_imgui)