find_package(Catch2 3 REQUIRED CONFIG)

add_executable(PruneTests
    editor_command_history_tests.cpp
    authoring_contract_tests.cpp
    ${PROJECT_SOURCE_DIR}/src/prune/editor/editor_command.cpp
    ${PROJECT_SOURCE_DIR}/src/prune/editor/editor_actions.cpp
    ${PROJECT_SOURCE_DIR}/src/prune/core/input.cpp
    ${PROJECT_SOURCE_DIR}/src/prune/scene/world_scene.cpp
    ${PROJECT_SOURCE_DIR}/src/prune/scene/scene_camera.cpp
    ${PROJECT_SOURCE_DIR}/src/prune/scene/scene_renderer.cpp
    ${PROJECT_SOURCE_DIR}/src/prune/scene/scene_interaction.cpp
    ${PROJECT_SOURCE_DIR}/src/prune/scene/scene_serializer.cpp
    ${PROJECT_SOURCE_DIR}/src/prune/scene/game_object.cpp
    ${PROJECT_SOURCE_DIR}/src/prune/scene/game_object_manager.cpp
    ${PROJECT_SOURCE_DIR}/src/prune/scene/editor_selection.cpp
    ${PROJECT_SOURCE_DIR}/src/prune/scene/collision.cpp
    ${PROJECT_SOURCE_DIR}/src/prune/resources/sprites.cpp
)

target_include_directories(PruneTests PRIVATE
    ${PROJECT_SOURCE_DIR}/src
)

target_link_libraries(PruneTests PRIVATE
    Catch2::Catch2WithMain
    SDL2::SDL2
    $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>
    yaml-cpp::yaml-cpp
)

if(MSVC)
    target_compile_options(PruneTests PRIVATE /W4 /permissive-)
else()
    target_compile_options(PruneTests PRIVATE -Wall -Wextra -Wpedantic)
endif()

prune_enable_sanitizers(PruneTests)

include(Catch)
catch_discover_tests(PruneTests)
