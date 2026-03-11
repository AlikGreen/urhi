cmake_minimum_required(VERSION 3.29)
project(urhi)

set(CMAKE_CXX_STANDARD 23)

option(URHI_ENABLE_VALIDATION "Enable URHI runtime validation" OFF)

set(DEPS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/dependencies")

set(ENABLE_OPT OFF CACHE BOOL "" FORCE)
add_subdirectory(${DEPS_DIR}/glm)
add_subdirectory(${DEPS_DIR}/glfw)
add_subdirectory(${DEPS_DIR}/glad)
add_subdirectory(${DEPS_DIR}/spirv-cross)
add_subdirectory(${DEPS_DIR}/clogr)
add_subdirectory(${DEPS_DIR}/vk-bootstrap)
add_subdirectory(${DEPS_DIR}/rapidyaml)
add_subdirectory(${DEPS_DIR}/vma)

find_package(Vulkan REQUIRED)

if(WIN32)
    set(SLANG_BIN_DIR "${DEPS_DIR}/slang/windows-x64/bin")
    set(SLANG_LIB_DIR "${DEPS_DIR}/slang/windows-x64/lib")
    set(SLANG_LIBRARY "${SLANG_LIB_DIR}/slang.lib")
    file(GLOB SLANG_DLLS "${SLANG_BIN_DIR}/*.dll")
#elseif(APPLE)
#    set(SLANG_LIB_DIR "${DEPS_DIR}/slang/bin/macos")
#else()
#    set(SLANG_LIB_DIR "${DEPS_DIR}/slang/bin/linux-x64")
endif()

### URHI
file(GLOB_RECURSE
        SRC_FILES
        CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
)

add_library(urhi STATIC ${SRC_FILES})

if(URHI_ENABLE_VALIDATION)
    target_compile_definitions(urhi PUBLIC URHI_ENABLE_VALIDATION)
endif()

target_compile_definitions(urhi PRIVATE VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1)
target_compile_definitions(urhi PRIVATE GLFW_INCLUDE_NONE)

target_include_directories(urhi PUBLIC src/)
target_include_directories(urhi PUBLIC include/ "${DEPS_DIR}/slang/include")
target_link_libraries(urhi PUBLIC
        glfw
        glm::glm
        glad
        clogr
        ${SLANG_LIBRARY}
        spirv-cross-c
        vk-bootstrap::vk-bootstrap
        Vulkan::Vulkan
        VulkanMemoryAllocator)

file(COPY ${SLANG_DLLS}
        DESTINATION "${CMAKE_BINARY_DIR}")

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
file(GLOB IMGUZMO_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/imguizmo/*.cpp")
list(APPEND IMGUI_SOURCES ${IMGUZMO_SOURCES})

add_library(imgui_static STATIC ${IMGUI_SOURCES})

target_include_directories(imgui_static PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/imgui"
        "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/imguizmo"
)

### IMGUI BACKEND
add_library(urhi_imgui STATIC
        backends/imgui/imGuiController.cpp
        backends/imgui/imGuiController.h
        backends/imgui/imGuiConfig.h
        backends/imgui/imGuiExtensions.h
        backends/imgui/imGuiExtensions.cpp)

target_include_directories(urhi_imgui PUBLIC backends/)
target_link_libraries(urhi_imgui PUBLIC urhi imgui_static ryml::ryml c4core::c4core)

target_compile_definitions(urhi_imgui PRIVATE
        IMGUI_USER_CONFIG="imgui/imGuiConfig.h"
        IMGUI_DISABLE_OBSOLETE_FUNCTIONS
)

add_library(urhi::imgui ALIAS urhi_imgui)

### BASIC EXAMPLE
add_executable(urhi_basic_example examples/basic.cpp)
target_link_libraries(urhi_basic_example PUBLIC urhi)

if(MINGW)
    target_link_options(urhi_basic_example PRIVATE
            -static-libgcc
            -static-libstdc++
            -static
    )
endif()

### IMGUI EXAMPLE
add_executable(urhi_imgui_example examples/imgui.cpp)
target_link_libraries(urhi_imgui_example PUBLIC urhi::imgui)


if(MINGW)
    target_link_options(urhi_imgui_example PRIVATE
            -static-libgcc
            -static-libstdc++
            -static
    )
endif()