#pragma once

#include <optional>

#include "imgui.h"

namespace prune::tooling::editor {

  template <typename T>
  class EditTracker {
  public:
    void capture_if_activated(const T& before) {
      if (ImGui::IsItemActivated()) {
        m_before = before;
      }
    }

    [[nodiscard]] std::optional<T> take_before_if_deactivated_after_edit() {
      if (!ImGui::IsItemDeactivatedAfterEdit() || !m_before.has_value()) {
        return std::nullopt;
      }

      std::optional<T> before = std::move(m_before);
      m_before.reset();
      return before;
    }

    void reset() noexcept { m_before.reset(); }

  private:
    std::optional<T> m_before;
  };

} // namespace prune::tooling::editor
