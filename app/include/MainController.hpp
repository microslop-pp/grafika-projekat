#ifndef MAINCONTROLLER_HPP
#define MAINCONTROLLER_HPP

#include "glm/ext/vector_float3.hpp"
#include <engine/core/Controller.hpp>

namespace app {

class MainController : public engine::core::Controller {
private:
    enum LightEvent {
        IDLE,
        WAITINGA,
        WAITINGB
    };

    LightEvent m_light_event = IDLE;
    float m_light_event_timer = 0.0f;
    float m_light_intensity = 1.0f;

    static constexpr float LIGHT_M_SECONDS = 2.0f;
    static constexpr float LIGHT_N_SECONDS = 3.0f;

    void update_light_event();

    glm::vec3 m_light_pos = glm::vec3(2.0f, 3.0f, -2.0f);

    int m_post_effect = 0;

    void set_effect();
    
    void initialize() override;

    bool loop() override;

    void draw_skybox();

    void update_light();

    void draw_light_marker();

    void update_camera();

    void draw_tower_b();

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