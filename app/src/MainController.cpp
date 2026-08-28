
#include "engine/core/Controller.hpp"
#include "engine/graphics/Camera.hpp"
#include "engine/platform/Input.hpp"
#include "glm/ext/vector_float3.hpp"
#include <MainController.hpp>
#include <memory>
#include <spdlog/spdlog.h>
#include <engine/resources/ResourcesController.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <engine/platform/PlatformController.hpp>
#include <engine/graphics/GraphicsController.hpp>

namespace app {

    class MainPlatformEventObserver : public engine::platform::PlatformEventObserver {
    public:
        void on_mouse_move(engine::platform::MousePosition position) override;
    private:
        int m_skip_frames = 2; // for some reason mouse position jumps for the first 2 frames
    };

    void MainPlatformEventObserver::on_mouse_move(engine::platform::MousePosition position) {
        if (m_skip_frames > 0) {
            m_skip_frames--;
            return;
        }
        auto camera = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();
        camera->rotate_camera(position.dx, position.dy);
    }

    bool MainController::loop() {
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        if (platform->key(engine::platform::KEY_ESCAPE).is_down()) {
            return false;
        }
        return true;
    }

    void MainController::initialize() {
        spdlog::info("MainController initialized!");
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        platform->register_platform_event_observer(std::make_unique<MainPlatformEventObserver>());
        platform->set_enable_cursor(false);
        engine::graphics::OpenGL::enable_depth_testing();
    }

    void MainController::update_light() {
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        float dt = platform->dt();
        float speed = 3.0f;

        if (platform->key(engine::platform::KeyId::KEY_RIGHT).is_down()) {
            m_light_pos.x += speed * dt;
        }
        if (platform->key(engine::platform::KeyId::KEY_LEFT).is_down()) {
            m_light_pos.x -= speed * dt;
        }
        if (platform->key(engine::platform::KeyId::KEY_UP).is_down()) {
            m_light_pos.z -= speed * dt;
        }
        if (platform->key(engine::platform::KeyId::KEY_DOWN).is_down()) {
            m_light_pos.z += speed * dt;
        }
    }

    void MainController::update_light_event() {
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        float dt = platform->dt();

        if (m_light_event == IDLE && platform->key(engine::platform::KeyId::KEY_L).state() == engine::platform::Key::State::JustPressed) {
            m_light_event = WAITINGA;
            m_light_event_timer = 0.0f;
            spdlog::info("waiting {}s", LIGHT_M_SECONDS);
        }

        switch (m_light_event) {
            case WAITINGA:
                m_light_event_timer += dt;
                if (m_light_event_timer >= LIGHT_M_SECONDS) {
                    m_light_intensity = 0.0f;
                    m_light_event = WAITINGB;
                    m_light_event_timer = 0.0f;
                    spdlog::info("waiting {}s", LIGHT_N_SECONDS);
                }
                break;

            case WAITINGB:
                m_light_event_timer += dt;
                if (m_light_event_timer >= LIGHT_N_SECONDS) {
                    m_light_intensity = 1.0f;
                    m_light_event = IDLE;
                }
                break;

            case IDLE:
                break;
        }
    }

    void MainController::draw_light_marker() {
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
        engine::resources::Model* light_cube = resources->model("light_cube");
        engine::resources::Shader* shader = resources->shader("basic");

        auto light_pos = m_light_pos;

        shader->use();
        shader->set_mat4("projection", graphics->projection_matrix());
        shader->set_mat4("view", graphics->camera()->view_matrix());

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, light_pos);
        model = glm::scale(model, glm::vec3(0.1f));
        shader->set_mat4("model", model);
        shader->set_mat3("normal_matrix", glm::transpose(glm::inverse(glm::mat3(model))));

        light_cube->draw(shader);
    }

    float rotate_tower_b(float rotation_angle, bool *finished_rotation) {
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();

        if (!(*finished_rotation) || platform->key(engine::platform::KeyId::KEY_R).state() == engine::platform::Key::State::JustPressed) {
            rotation_angle += 1.0f;
            *finished_rotation = false;
            if (rotation_angle >= 360.0f) {
                rotation_angle = 0.0f;
                *finished_rotation = true;
            }
        }

        return rotation_angle;
    }

    void MainController::draw_tower_b() {
        // Model
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
        engine::resources::Model* tower_b = resources->model("tower_b");

        // Shader
        engine::resources::Shader* shader = resources->shader("basic");

        shader->use();
        shader->set_mat4("projection", graphics->projection_matrix());
        shader->set_mat4("view", graphics->camera()->view_matrix());

        static float rotation_angle = 0.0f;
        static bool finished_rotation = true;
        
        rotation_angle = rotate_tower_b(rotation_angle, &finished_rotation);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -6.0f));
        model = glm::rotate(model, glm::radians(rotation_angle), glm::vec3(0.0f, 1.0f, 0.0f));
        shader->set_mat4("model", model);

        // light_color_dir is light colour for directional lighting

        shader->set_vec3("light_pos", m_light_pos);
        shader->set_vec3("light_direction", glm::vec3(2.0f, -3.0f, 2.0f));
        shader->set_vec3("light_color_dir", m_light_intensity * glm::vec3(1.0f, 1.0f, 1.0f));
        shader->set_vec3("light_color", m_light_intensity * glm::vec3(0.0f, 1.0f, 0.0f));
        shader->set_vec3("view_pos", graphics->camera()->Position);
        shader->set_mat3("normal_matrix", glm::transpose(glm::inverse(glm::mat3(model))));

        tower_b->draw(shader);
    }

    void MainController::draw_skybox() {
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto skybox = resources->skybox("skybox");
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
        auto shader = resources->shader("skyboxshader");
        graphics->draw_skybox(shader, skybox);
    }

    void MainController::update_camera() {
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        auto graphics = engine::graphics::GraphicsController::get<engine::graphics::GraphicsController>();
        auto camera = graphics->camera();

        glm::vec3 flat_front = glm::normalize(glm::vec3(camera->Front.x, 0.0f, camera->Front.z));
        
        float dt = platform->dt();
        if(platform->key(engine::platform::KeyId::KEY_W).is_down()) {
            camera->Position += flat_front * camera->MovementSpeed * dt;
        }
        if(platform->key(engine::platform::KeyId::KEY_S).is_down()) {
            camera->Position -= flat_front * camera->MovementSpeed * dt;
        }
        if(platform->key(engine::platform::KeyId::KEY_A).is_down()) {
            camera->move_camera(engine::graphics::Camera::Movement::LEFT, dt);
        }
        if(platform->key(engine::platform::KeyId::KEY_D).is_down()) {
            camera->move_camera(engine::graphics::Camera::Movement::RIGHT, dt);
        }
        if(platform->key(engine::platform::KeyId::KEY_LEFT_SHIFT).is_down()) {
            camera->Position -= glm::vec3(0.0f, 1.0f, 0.0f) * camera->MovementSpeed * dt;
        }
        if(platform->key(engine::platform::KeyId::KEY_SPACE).is_down()) {
            camera->Position -= glm::vec3(0.0f, -1.0f, 0.0f) * camera->MovementSpeed * dt;
        }
    }

    void MainController::update() {
        update_camera();
        update_light();
        update_light_event();
    }

    void MainController::begin_draw() {
        engine::graphics::OpenGL::clear_buffers();
    }

    void MainController::draw() {
        draw_tower_b();
        draw_light_marker();
        draw_skybox();
    }

    void MainController::end_draw() {
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        platform->swap_buffers();
    }
}