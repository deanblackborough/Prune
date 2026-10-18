#include <string_view>
#include <variant>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "prune/editor/editor_command.hpp"

namespace {

  prune::EditorCommand make_command(std::string_view label) {
    prune::EditorCommand command{};
    command.label = label;
    return command;
  }

  prune::GameObject make_object(prune::GameObjectId id) {
    prune::GameObject object{};
    object.identity.id = id;
    return object;
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

TEST_CASE("make_object_change produces an ObjectChange payload",
          "[editor][command-payload]") {
  const prune::GameObject before = make_object(1);
  prune::GameObject after = before;
  after.transform.x = 10.0f;

  const prune::EditorCommand command =
      prune::make_object_change("Move object", before, after, "detail");

  CHECK(command.label == "Move object");
  CHECK(command.detail == "detail");
  CHECK(command.makes_dirty);
  REQUIRE(std::holds_alternative<prune::ObjectChange>(command.payload));

  const auto& payload = std::get<prune::ObjectChange>(command.payload);
  CHECK(payload.before.identity.id == 1);
  CHECK(payload.after.transform.x == 10.0f);

  CHECK(prune::affected_ids(command) == std::vector<prune::GameObjectId>{1});
}

TEST_CASE("make_object_create produces an ObjectCreate payload",
          "[editor][command-payload]") {
  const prune::EditorCommand command =
      prune::make_object_create(make_object(2), "Box");

  REQUIRE(std::holds_alternative<prune::ObjectCreate>(command.payload));
  CHECK(std::get<prune::ObjectCreate>(command.payload).object.identity.id ==
        2);
  CHECK(prune::affected_ids(command) == std::vector<prune::GameObjectId>{2});
}

TEST_CASE("make_object_delete produces an ObjectDelete payload",
          "[editor][command-payload]") {
  const prune::EditorCommand command =
      prune::make_object_delete(make_object(3), "Box");

  REQUIRE(std::holds_alternative<prune::ObjectDelete>(command.payload));
  CHECK(std::get<prune::ObjectDelete>(command.payload).object.identity.id ==
        3);
  CHECK(prune::affected_ids(command) == std::vector<prune::GameObjectId>{3});
}

TEST_CASE("make_objects_change produces an ObjectsChange payload",
          "[editor][command-payload]") {
  const std::vector<prune::GameObject> before = {make_object(4),
                                                 make_object(5)};
  const std::vector<prune::GameObject> after = before;

  const prune::EditorCommand command =
      prune::make_objects_change("Move objects", before, after, "2 objects");

  REQUIRE(std::holds_alternative<prune::ObjectsChange>(command.payload));
  CHECK(std::get<prune::ObjectsChange>(command.payload).after.size() == 2);
  CHECK(prune::affected_ids(command) ==
        std::vector<prune::GameObjectId>{4, 5});
}

TEST_CASE("make_objects_delete produces an ObjectsDelete payload",
          "[editor][command-payload]") {
  const std::vector<prune::GameObject> deleted = {make_object(6),
                                                   make_object(7)};

  const prune::EditorCommand command =
      prune::make_objects_delete(deleted, "2 objects");

  REQUIRE(std::holds_alternative<prune::ObjectsDelete>(command.payload));
  CHECK(std::get<prune::ObjectsDelete>(command.payload).objects.size() == 2);
  CHECK(prune::affected_ids(command) ==
        std::vector<prune::GameObjectId>{6, 7});
}

TEST_CASE("make_viewport_change produces a ViewportChange payload and does "
          "not mark dirty",
          "[editor][command-payload]") {
  const prune::Camera before{};
  prune::Camera after{};
  after.x = 100.0f;

  const prune::EditorCommand command =
      prune::make_viewport_change("Move viewport", before, after, "pan");

  REQUIRE(std::holds_alternative<prune::ViewportChange>(command.payload));
  CHECK_FALSE(command.makes_dirty);
  CHECK(std::get<prune::ViewportChange>(command.payload).after.x == 100.0f);
  CHECK(prune::affected_ids(command).empty());
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
