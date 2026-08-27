#ifndef MAINCONTROLLER_HPP
#define MAINCONTROLLER_HPP

#include "glm/ext/vector_float3.hpp"
#include <engine/core/Controller.hpp>

namespace app {

class MainController : public engine::core::Controller {
private:
    glm::vec3 m_light_pos = glm::vec3(2.0f, 3.0f, -2.0f);
    
    void initialize() override;

    bool loop() override;

    void draw_skybox();

    void update_light();

    void draw_light_marker();

    void update_camera();

    void draw_corridor();

    void update() override;

    void begin_draw() override;

    void draw() override;

    void end_draw() override;

public:
    std::string_view name() const override {
        return "app::MainController";
    }
};

} // app

#endif //MAINCONTROLLER_HPP