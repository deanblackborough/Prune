cmake_minimum_required(VERSION 3.25)
project(Prune)

include(CTest)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(PRUNE_SANITIZE "Build with AddressSanitizer (and UndefinedBehaviorSanitizer on Clang/GCC)" OFF)

if(PRUNE_SANITIZE AND MSVC)
    # MSVC AddressSanitizer is incompatible with the runtime checks CMake adds to Debug builds.
    string(REGEX REPLACE "/RTC[1csu]+" "" CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
    string(REGEX REPLACE "/RTC[1csu]+" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
endif()

function(prune_enable_sanitizers target)
    if(NOT PRUNE_SANITIZE)
        return()
    endif()

    if(MSVC)
        target_compile_options(${target} PRIVATE /fsanitize=address)
        target_link_options(${target} PRIVATE /INCREMENTAL:NO)
        # vcpkg's SDL2 / Catch2 are not ASan-instrumented; disable the STL container
        # annotations so mixed-annotation objects can still link (MSVC ASan limitation).
        target_compile_definitions(${target} PRIVATE _DISABLE_STL_ANNOTATION)
    else()
        target_compile_options(${target} PRIVATE
            -fsanitize=address,undefined
            -fno-sanitize-recover=all
            -fno-omit-frame-pointer)
        target_link_options(${target} PRIVATE -fsanitize=address,undefined)
    endif()
endfunction()

function(prune_set_warnings target)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4 /permissive-)
    else()
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endfunction()

find_package(SDL2 REQUIRED CONFIG)
find_package(SDL2_image REQUIRED CONFIG)
find_package(yaml-cpp REQUIRED CONFIG)

# Runtime + scene behaviour. No Dear ImGui: this is the half of the app that
# a headless build, a sanitizer smoke run, or a future playable export can
# link without pulling in editor presentation.
add_library(prune_runtime STATIC
        src/prune/core/window.cpp
        src/prune/core/time.cpp
        src/prune/core/input.cpp

        src/prune/audio/audio_system.cpp

        src/prune/editor/editor_command.cpp
        src/prune/editor/editor_actions.cpp

        src/prune/scene/game_object.cpp
        src/prune/scene/game_object_manager.cpp
        src/prune/scene/editor_selection.cpp
        src/prune/scene/collision.cpp

        src/prune/scene/scene_factory.cpp
        src/prune/scene/scene_renderer.cpp
        src/prune/scene/scene_camera.cpp
        src/prune/scene/world_scene.cpp
        src/prune/scene/scene_interaction.cpp
        src/prune/scene/scene_serializer.cpp

        src/prune/scene/simple_shooter/simple_shooter_scene.cpp
        src/prune/scene/simple_shooter/simple_shooter_factory.cpp
        src/prune/scene/simple_shooter/simple_shooter_behaviour.cpp
        src/prune/scene/simple_shooter/simple_shooter_player_controller.cpp
        src/prune/scene/simple_shooter/simple_shooter_concepts.cpp
        src/prune/scene/simple_shooter/simple_shooter_serializer.cpp

        src/prune/scene/platformer/platformer_scene.cpp
        src/prune/scene/platformer/platformer_factory.cpp
        src/prune/scene/platformer/platformer_behaviour.cpp
        src/prune/scene/platformer/platformer_concepts.cpp
        src/prune/scene/platformer/platformer_serializer.cpp

        src/prune/scene/artillery/artillery_scene.cpp
        src/prune/scene/artillery/artillery_factory.cpp
        src/prune/scene/artillery/artillery_behaviour.cpp
        src/prune/scene/artillery/artillery_concepts.cpp
        src/prune/scene/artillery/artillery_serializer.cpp

        src/prune/resources/sprites.cpp
)

target_include_directories(prune_runtime PUBLIC
        src
)

target_link_libraries(prune_runtime PUBLIC
        $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
        $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>
        yaml-cpp::yaml-cpp
)

prune_set_warnings(prune_runtime)
prune_enable_sanitizers(prune_runtime)

# Editor shell, tooling panels, and scene editors. Links Dear ImGui and
# depends on prune_runtime; nothing in prune_runtime may depend back on this.
add_library(prune_editor STATIC
        src/prune/scene/scene_editor_factory.cpp
        src/prune/scene/world_scene_editor.cpp
        src/prune/scene/simple_shooter/simple_shooter_editor.cpp
        src/prune/scene/platformer/platformer_editor.cpp
        src/prune/scene/artillery/artillery_editor.cpp

        src/prune/tooling/theme.cpp
        src/prune/tooling/ui.cpp
        src/prune/tooling/outliner.cpp
        src/prune/tooling/inspector.cpp
        src/prune/tooling/command_history.cpp
        src/prune/tooling/sprite_picker.cpp
        src/prune/tooling/editor/tracked_property_table.cpp
        src/prune/tooling/editor/tracked_settings_table.cpp
        src/prune/tooling/controls.cpp
        src/prune/tooling/options.cpp
        src/prune/tooling/stats.cpp
        src/prune/tooling/editor_layout.cpp
        src/prune/tooling/simple_shooter/simple_shooter.cpp
        src/prune/tooling/platformer/platformer.cpp
        src/prune/tooling/artillery/artillery.cpp

        src/prune/tooling/imgui/property_table.cpp
        src/prune/tooling/imgui/layout.cpp
        src/prune/tooling/imgui/shortcut.cpp

        external/imgui/imgui.cpp
        external/imgui/imgui_draw.cpp
        external/imgui/imgui_tables.cpp
        external/imgui/imgui_widgets.cpp
        external/imgui/imgui_demo.cpp
        external/imgui/backends/imgui_impl_sdl2.cpp
        external/imgui/backends/imgui_impl_sdlrenderer2.cpp
)

target_include_directories(prune_editor PUBLIC
        external/imgui
        external/imgui/backends
)

target_link_libraries(prune_editor PUBLIC
        prune_runtime
)

prune_set_warnings(prune_editor)
prune_enable_sanitizers(prune_editor)

# Thin executable: wires the runtime scenes to the editor shell. Nothing
# scene- or tooling-specific belongs here.
add_executable(Prune
        src/main.cpp
        src/prune/app/app.cpp
)

target_link_libraries(Prune PRIVATE
        # prune_editor links prune_runtime PUBLIC, so this pulls in both.
        prune_editor
)

add_custom_command(TARGET Prune POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets
    $<TARGET_FILE_DIR:Prune>/assets
)

prune_set_warnings(Prune)
prune_enable_sanitizers(Prune)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
