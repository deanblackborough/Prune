#include <stdexcept>
#include <filesystem>
#include <memory>
#include <string>
#include <utility>

#include <SDL2/SDL.h>
#include <SDL_image.h>

#include "imgui.h"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_sdlrenderer2.h"

#include "prune/app/app.hpp"
#include "prune/core/time.hpp"
#include "prune/scene/scene_factory.hpp"
#include "prune/tooling/theme.hpp"
#include "prune/tooling/ui.hpp"

namespace prune {

    App::App(const AppConfig& config)
        : m_fixed_timestep(config.fixed_timestep)
    {
        init_sdl();

        m_window = std::make_unique<Window>(config.window);
        m_input = std::make_unique<Input>();
        m_time = std::make_unique<Time>();
        m_scene = SceneFactory::create(SceneType::Platformer, m_window->width(), m_window->height());
        m_scene->new_scene();
        m_scene->on_enter();
        m_ui = std::make_unique<Ui>();

        init_imgui();
    }

    App::~App()
    {
        if (m_scene) {
            m_scene->on_exit();
        }

        shutdown_imgui();

        // Clean up in reverse order
        m_scene.reset();
        m_ui.reset();
        m_input.reset();
        m_time.reset();
        m_window.reset();

        shutdown_sdl();
    }

    void App::run()
    {
        m_running = true;

        while (m_running) {
            process_events();

            const ImGuiIO& io = ImGui::GetIO();
            if (m_input->was_key_pressed(SDL_SCANCODE_ESCAPE) && !io.WantCaptureKeyboard && !io.WantTextInput) {
                m_running = false;
                break;
            }

            m_time->tick();

            float frame_time = m_time->delta_seconds();

            if (frame_time > 0.25f) {
                frame_time = 0.25f;
            }

            m_accumulator += frame_time;

            begin_imgui_frame();

            if (m_scene && m_ui && m_window) {
                bool new_scene_requested = false;
                SceneType new_scene_type = SceneType::SimpleShooter;
                bool load_scene_requested = false;

                m_ui->build(
                    *m_scene,
                    m_window->renderer(),
                    new_scene_requested,
                    new_scene_type,
                    load_scene_requested
                );

                if (new_scene_requested) {
                    m_scene->on_exit();

                    m_scene = SceneFactory::create(
                        new_scene_type,
                        m_window->width(),
                        m_window->height()
                    );

                    m_scene->new_scene();
                    m_scene->on_enter();
                    m_ui->set_file_status("Created new scene", false);
                }

                if (load_scene_requested) {
                    std::string error;
                    const std::string scene_file_path{ m_scene->default_file_path() };

                    std::unique_ptr<Scene> loaded_scene = SceneFactory::create_from_file(
                        scene_file_path,
                        m_window->width(),
                        m_window->height(),
                        error
                    );

                    if (loaded_scene) {
                        m_scene->on_exit();
                        m_scene = std::move(loaded_scene);
                        m_scene->on_enter();
                        m_ui->set_file_status("Loaded scene from " + scene_file_path, false);
                    }
                    else {
                        m_ui->set_file_status("Load failed: " + error, true);
                    }
                }

                m_scene->update_editor(frame_time, *m_input);
            }

            while (m_accumulator >= m_fixed_timestep) {
                update(m_fixed_timestep);
                m_accumulator -= m_fixed_timestep;
            }

            render();
        }
    }

    void App::init_sdl()
    {
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
            throw std::runtime_error(std::string("Failed to initialize SDL: ") + SDL_GetError());
        }

        const int image_flags = IMG_INIT_PNG;

        if ((IMG_Init(image_flags) & image_flags) != image_flags) {
            const std::string error = std::string("Failed to initialise SDL_image PNG support: ") + IMG_GetError();
            SDL_Quit();
            throw std::runtime_error(error);
        }
    }

    void App::shutdown_sdl()
    {
        IMG_Quit();
        SDL_Quit();
    }

    void App::handle_event(const SDL_Event& event)
    {
        switch (event.type) {
            case SDL_QUIT:
                m_running = false;
                break;

            case SDL_WINDOWEVENT:
                switch (event.window.event) {
                case SDL_WINDOWEVENT_CLOSE:
                    m_running = false;
                    break;

                case SDL_WINDOWEVENT_SIZE_CHANGED:
                    if (m_window) {
                        m_window->refresh_size();
                    }
                    break;

                default:
                    break;
                }
                break;

            default:
                break;
        }
    }

    void App::process_events()
    {
        m_input->begin_frame();

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            handle_event(event);

            m_input->process_event(event);
        }
    }

    void App::update(float dt)
    {
        if (m_scene) {
            m_scene->update(dt, *m_input);
        }
    }

    void App::render()
    {
        SDL_Renderer* renderer = m_window->renderer();

        SDL_SetRenderDrawColor(renderer, 18, 14, 24, 255);
        SDL_RenderClear(renderer);

        if (m_scene && m_ui) {
            m_ui->render_scene_viewport_content(*m_scene, renderer);
        }

        ImGui::Render();
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);

        SDL_RenderPresent(renderer);
    }

    void App::init_imgui() {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO();

        ImFontConfig font_config;
        font_config.OversampleH = 3;
        font_config.OversampleV = 2;
        font_config.PixelSnapH = true;

        const std::filesystem::path font_path =
            std::filesystem::current_path() / "assets" / "fonts" / "JetBrainsMonoNL-Regular.ttf";

        ImFont* font = io.Fonts->AddFontFromFileTTF(
            font_path.string().c_str(),
            16.0f,
            &font_config
        );

        if (font == nullptr) {
            io.Fonts->AddFontDefault();
        }

        io.FontGlobalScale = 1.0f;

        ImGui::StyleColorsDark();
		prune::tooling::apply_editor_theme(prune::tooling::EditorTheme::PrunePurple);

        if (!ImGui_ImplSDL2_InitForSDLRenderer(
            m_window->sdl_window(),
            m_window->renderer())) {
            ImGui::DestroyContext();
            throw std::runtime_error("Failed to initialise ImGui SDL2 backend");
        }

        if (!ImGui_ImplSDLRenderer2_Init(m_window->renderer())) {
            ImGui_ImplSDL2_Shutdown();
            ImGui::DestroyContext();
            throw std::runtime_error("Failed to initialise ImGui SDL renderer backend");
        }
    }

    void App::shutdown_imgui()
    {
        ImGui_ImplSDLRenderer2_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
    }

    void App::begin_imgui_frame()
    {
        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
    }
}
