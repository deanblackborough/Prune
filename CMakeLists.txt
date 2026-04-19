cmake_minimum_required(VERSION 3.20)
project(Prune)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SDL2 REQUIRED CONFIG)
find_package(yaml-cpp REQUIRED CONFIG)

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
        src/prune/scene/sandbox_scene_editor.cpp
        src/prune/scene/sandbox_scene_serialization.cpp

        src/prune/tooling/ui.cpp
        src/prune/tooling/outliner.cpp
        src/prune/tooling/inspector.cpp
        src/prune/tooling/controls.cpp
        src/prune/tooling/options.cpp
        src/prune/tooling/stats.cpp

        src/prune/tooling/imgui/property_table.cpp
        src/prune/tooling/imgui/layout.cpp

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
        yaml-cpp::yaml-cpp
)

if(MSVC)
    target_compile_options(Prune PRIVATE /W4 /permissive-)
else()
    target_compile_options(Prune PRIVATE -Wall -Wextra -Wpedantic)
endif()