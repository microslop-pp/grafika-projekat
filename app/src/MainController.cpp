#include <MainController.hpp>
#include <spdlog/spdlog.h>
#include <engine/resources/ResourcesController.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <engine/platform/PlatformController.hpp>
#include <engine/graphics/GraphicsController.hpp>

auto rotation_angle = 0.0f;

namespace app {
    void MainController::initialize() {
        spdlog::info("MainController initialized!");
        engine::graphics::OpenGL::enable_depth_testing();
    }

    void MainController::draw_corridor() {
        // Model
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
        engine::resources::Model* corridor = resources->model("corridor");

        // Shader
        engine::resources::Shader* shader = resources->shader("basic");

        shader->use();
        shader->set_mat4("projection", graphics->projection_matrix());
        glm::mat4 view = glm::rotate(graphics->camera()->view_matrix(), glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        shader->set_mat4("view", view);


        rotation_angle += 0.5f; // Adjust this value to change speed
        if (rotation_angle >= 360.0f) {
            rotation_angle -= 360.0f;
        }

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 6.0f));
        model = glm::rotate(model, glm::radians(rotation_angle), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.3f));
        shader->set_mat4("model", model);
        corridor->draw(shader);
    }

    void MainController::begin_draw() {
        engine::graphics::OpenGL::clear_buffers();

    }

    void MainController::draw() {
        draw_corridor();
    }

    void MainController::end_draw() {
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        platform->swap_buffers();
    }
}