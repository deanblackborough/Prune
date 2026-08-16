find_package(Catch2 3 REQUIRED CONFIG)

add_executable(PruneTests
    editor_command_history_tests.cpp
    ${PROJECT_SOURCE_DIR}/src/prune/editor/editor_command.cpp
)

target_include_directories(PruneTests PRIVATE
    ${PROJECT_SOURCE_DIR}/src
)

target_link_libraries(PruneTests PRIVATE
    Catch2::Catch2WithMain
    SDL2::SDL2
)

if(MSVC)
    target_compile_options(PruneTests PRIVATE /W4 /permissive-)
else()
    target_compile_options(PruneTests PRIVATE -Wall -Wextra -Wpedantic)
endif()

include(Catch)
catch_discover_tests(PruneTests)
