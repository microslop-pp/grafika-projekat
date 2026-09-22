#ifndef MAINCONTROLLER_HPP
#define MAINCONTROLLER_HPP

#include "glm/ext/vector_float3.hpp"
#include <engine/core/Controller.hpp>
#include <string>
#include <vector>

namespace app {

class MainController : public engine::core::Controller {
private:
    enum LightEvent {
        IDLE,
        WAITINGA,
        WAITINGB
    };

    struct Tower { std::string name; glm::vec3 position; };

    std::vector<Tower> m_towers = {
        {"tower_a", glm::vec3(-3.0f, -1.0f, -6.0f)},
        {"tower_b", glm::vec3( 0.0f, -1.0f, -6.0f)},
        {"tower_c", glm::vec3( 3.0f, -1.0f, -6.0f)},
        {"tower_d", glm::vec3(5.0f, -1.0f, -6.0f)},
        {"tower_d", glm::vec3(-3.0f, -1.0f, 1.0f)},
        {"tower_a", glm::vec3( 0.0f, -1.0f, 1.0f)},
        {"tower_c", glm::vec3( 3.0f, -1.0f, 1.0f)},
        {"tower_b", glm::vec3(5.0f, -1.0f, 1.0f)},
        
    };

    LightEvent m_light_event = IDLE;
    float m_light_event_timer{0.0f};
    float m_light_intensity{1.0f};

    static constexpr float LIGHT_M_SECONDS = 2.0f;
    static constexpr float LIGHT_N_SECONDS = 3.0f;

    void update_light_event();

    struct Light {
        glm::vec3 pos;
        glm::vec3 color;
    };

    Light m_light = {
        glm::vec3(2.0f, 4.0f, -2.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    };


    void update_light_color();

    float m_tower_rotation_angle{0.0f};
    bool m_tower_finished_rotation{true};

    int m_post_effect = 0;

    void set_effect();
    
    void initialize() override;

    bool loop() override;

    void draw_skybox();

    void draw_ground();

    void update_light();

    void draw_light_marker();

    void update_camera();

    void draw_tower(std::string &model_name, glm::vec3 &position, float rotation_deg);
    void draw_towers();

    void draw_shadow_pass();

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