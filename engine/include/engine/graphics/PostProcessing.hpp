#ifndef POSTPROCESSING_HPP
#define POSTPROCESSING_HPP

namespace engine::resources {
class Shader;
}// namespace engine::resources

namespace engine::graphics {
class PostProcessing {
public:
    PostProcessing() = default;

    PostProcessing(int width, int height);

    void setup(int width, int height);

    void begin() const;

    void end() const;

    void draw_quad(const resources::Shader *shader) const;

    bool needs_resize(int width, int height) const;

    unsigned int fbo() const {
        return m_fbo;
    }

    unsigned int texture() const {
        return m_texture_colorbuffer;
    }

    unsigned int vao() const {
        return m_quad_vao;
    }

    void destroy();

private:
    unsigned int m_fbo{0};
    unsigned int m_texture_colorbuffer{0};
    unsigned int m_rbo{0};
    unsigned int m_quad_vao{0};
    int m_width{0};
    int m_height{0};
};
}// namespace engine::graphics

#endif // POSTPROCESSING_HPP
