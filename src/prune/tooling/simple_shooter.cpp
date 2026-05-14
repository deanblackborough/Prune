#include "prune/tooling/simple_shooter.hpp"
#include "prune/tooling/imgui/layout.hpp"
#include "prune/tooling/imgui/property_table.hpp"

namespace prune {
	void SimpleShooter::draw(SimpleShooterOptions& options)
	{
        if (tooling::imgui::layout::collapsing_header("Game")) {
            if (tooling::imgui::property_table::begin("##game")) {
                tooling::imgui::property_table::slider_float("Enemy Speed", "enemy_speed", options.enemy_speed, 0.0f, 160.0f);
                tooling::imgui::property_table::slider_float("Bullet Speed", "bullet_speed", options.bullet_speed, 40.0f, 400.0f);
                tooling::imgui::property_table::slider_float("Bullet Lifetime", "bullet_lifetime", options.bullet_lifetime, 0.2f, 4.0f);
                tooling::imgui::property_table::end();
            }
        }
	}
}
