find_package(Catch2 3 REQUIRED CONFIG)

add_executable(PruneTests
    editor_command_history_tests.cpp
    authoring_contract_tests.cpp
)

target_link_libraries(PruneTests PRIVATE
    prune_runtime
    Catch2::Catch2WithMain
)

prune_set_warnings(PruneTests)
prune_enable_sanitizers(PruneTests)

include(Catch)
catch_discover_tests(PruneTests)
