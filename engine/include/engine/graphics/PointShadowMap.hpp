#ifndef POINTSHADOWMAP_HPP
#define POINTSHADOWMAP_HPP

#include <glm/glm.hpp>

namespace engine::resources {
class Shader;
}

namespace engine::graphics {

class PointShadowMap {
public:
    PointShadowMap() = default;

    void setup(int resolution = 1024);

    void begin(resources::Shader *shader, glm::vec3 &light_pos);

    void end(int screen_width, int screen_height);

    unsigned int texture() {
        return m_depth_cubemap; 
    }

    float far_plane() {
        return m_far_plane;
    }

    void destroy();

private:
    unsigned int m_fbo{0};
    unsigned int m_depth_cubemap{0};
    unsigned int m_resolution{1024};
    float m_near_plane{1.0f};
    float m_far_plane{25.0f};
};
}
#endif //POINTSHADOWMAP_HPP