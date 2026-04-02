cmake_minimum_required(VERSION 4.2)
project(Prune)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find required packages
find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)

# Dear ImGui setup
set(IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/imgui" CACHE PATH "Path to Dear ImGui")

if(NOT EXISTS "${IMGUI_DIR}/imgui.h")
    message(FATAL_ERROR "Dear ImGui not found at ${IMGUI_DIR}")
endif()

# Main executable
add_executable(Prune
        src/main.cpp
        src/prune/core/app.cpp
        src/prune/core/window.cpp
        src/prune/core/time.cpp
        src/prune/tooling/imgui_layer.cpp
        src/prune/core/app_config.hpp
        src/prune/core/time.hpp
        src/prune/core/window.cpp
        src/prune/core/window.cpp
        src/prune/core/app.hpp
        src/prune/core/app.hpp
        src/prune/core/app.cpp
        src/prune/core/app.cpp
        src/prune/tooling/imgui_layer.hpp
        src/prune/tooling/imgui_layer.hpp
        src/prune/tooling/imgui_layer.cpp
        src/prune/tooling/imgui_layer.cpp
)

add_library(imgui STATIC
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_demo.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${IMGUI_DIR}/backends/imgui_impl_sdl2.cpp
        ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui PUBLIC
        ${IMGUI_DIR}
        ${IMGUI_DIR}/backends
)

target_link_libraries(imgui PUBLIC
        SDL2::SDL2
        OpenGL::GL
)

target_include_directories(Prune PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(Prune PRIVATE
        SDL2::SDL2
        OpenGL::GL
        imgui
)

# Compiler warnings
if(MSVC)
    target_compile_options(Prune PRIVATE /W4 /permissive-)
else()
    target_compile_options(Prune PRIVATE -Wall -Wextra -Wpedantic)
endif()