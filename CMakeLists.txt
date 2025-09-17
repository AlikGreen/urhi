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
target_link_libraries(neonRHI PUBLIC glfw glm::glm glslang spirv-cross-c glad neonLog)