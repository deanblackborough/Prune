#include <string_view>

#include <catch2/catch_test_macros.hpp>

#include "prune/editor/editor_command.hpp"

namespace {

  prune::EditorCommand make_command(std::string_view label) {
    prune::EditorCommand command{};
    command.label = label;
    return command;
  }

} // namespace

TEST_CASE("A new editor command history is empty",
          "[editor][command-history]") {
  prune::EditorCommandHistory history;

  CHECK(history.empty());
  CHECK(history.count() == 0);
  CHECK(history.applied_count() == 0);
  CHECK_FALSE(history.can_undo());
  CHECK_FALSE(history.can_redo());
  CHECK(history.last_command() == nullptr);
  CHECK(history.undo_command() == nullptr);
  CHECK(history.redo_command() == nullptr);
}

TEST_CASE("Recording an editor command makes it undoable",
          "[editor][command-history]") {
  prune::EditorCommandHistory history;
  history.record(make_command("First command"));

  CHECK_FALSE(history.empty());
  CHECK(history.count() == 1);
  CHECK(history.applied_count() == 1);
  CHECK(history.can_undo());
  CHECK_FALSE(history.can_redo());

  const prune::EditorCommand* last = history.last_command();
  REQUIRE(last != nullptr);
  CHECK(last->label == "First command");
}

TEST_CASE("Undo and redo move the applied command cursor",
          "[editor][command-history]") {
  prune::EditorCommandHistory history;
  history.record(make_command("First command"));
  history.record(make_command("Second command"));

  const prune::EditorCommand* undone = history.undo_command();
  REQUIRE(undone != nullptr);
  CHECK(undone->label == "Second command");
  CHECK(history.count() == 2);
  CHECK(history.applied_count() == 1);
  CHECK(history.can_undo());
  CHECK(history.can_redo());

  const prune::EditorCommand* last = history.last_command();
  REQUIRE(last != nullptr);
  CHECK(last->label == "First command");

  const prune::EditorCommand* redone = history.redo_command();
  REQUIRE(redone != nullptr);
  CHECK(redone->label == "Second command");
  CHECK(history.applied_count() == 2);
  CHECK(history.can_undo());
  CHECK_FALSE(history.can_redo());
}

TEST_CASE("Recording after undo discards the redo branch",
          "[editor][command-history]") {
  prune::EditorCommandHistory history;
  history.record(make_command("First command"));
  history.record(make_command("Discarded command"));

  REQUIRE(history.undo_command() != nullptr);
  history.record(make_command("Replacement command"));

  CHECK(history.count() == 2);
  CHECK(history.applied_count() == 2);
  CHECK(history.can_undo());
  CHECK_FALSE(history.can_redo());

  const auto& commands = history.commands();
  REQUIRE(commands.size() == 2);
  CHECK(commands[0].label == "First command");
  CHECK(commands[1].label == "Replacement command");
}

TEST_CASE("Clearing editor command history resets commands and cursor",
          "[editor][command-history]") {
  prune::EditorCommandHistory history;
  history.record(make_command("First command"));
  history.record(make_command("Second command"));
  REQUIRE(history.undo_command() != nullptr);

  history.clear();

  CHECK(history.empty());
  CHECK(history.count() == 0);
  CHECK(history.applied_count() == 0);
  CHECK_FALSE(history.can_undo());
  CHECK_FALSE(history.can_redo());
  CHECK(history.last_command() == nullptr);
  CHECK(history.undo_command() == nullptr);
  CHECK(history.redo_command() == nullptr);
}
