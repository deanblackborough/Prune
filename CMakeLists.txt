cmake_minimum_required(VERSION 3.20)
project(Prune)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SDL2 REQUIRED CONFIG)

add_executable(Prune
        src/main.cpp

        src/prune/app/app.cpp

        src/prune/core/window.cpp
        src/prune/core/time.cpp
        src/prune/core/input.cpp

        src/prune/scene/game_object.cpp
        src/prune/scene/game_object_manager.cpp
        src/prune/scene/player_controller.cpp
        src/prune/scene/sandbox_scene.cpp

        src/prune/tooling/editor_ui.cpp

        external/imgui/imgui.cpp
        external/imgui/imgui_draw.cpp
        external/imgui/imgui_tables.cpp
        external/imgui/imgui_widgets.cpp
        external/imgui/imgui_demo.cpp
        external/imgui/backends/imgui_impl_sdl2.cpp
        external/imgui/backends/imgui_impl_sdlrenderer2.cpp
)

target_include_directories(Prune PRIVATE
        src
        external/imgui
        external/imgui/backends
)

target_link_libraries(Prune PRIVATE
        SDL2::SDL2
)

if(MSVC)
    target_compile_options(Prune PRIVATE /W4 /permissive-)
else()
    target_compile_options(Prune PRIVATE -Wall -Wextra -Wpedantic)
endif()