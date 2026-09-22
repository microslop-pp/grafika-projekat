#include <glad/glad.h>
#include <engine/graphics/OpenGL.hpp>
#include <engine/graphics/PostProcessing.hpp>
#include <engine/resources/Shader.hpp>
#include <engine/util/Errors.hpp>

namespace engine::graphics {

PostProcessing::PostProcessing(int width, int height) {
    setup(width, height);
}

void PostProcessing::destroy() {
    if (m_texture_colorbuffer != 0) {
        CHECKED_GL_CALL(glDeleteTextures, 1, &m_texture_colorbuffer);
        m_texture_colorbuffer = 0;
    } 

    if (m_rbo != 0) {
        CHECKED_GL_CALL(glDeleteRenderbuffers, 1, &m_rbo);
        m_rbo = 0;
    }

    if (m_fbo != 0) {
        CHECKED_GL_CALL(glDeleteFramebuffers, 1, &m_fbo);
        m_fbo = 0;
    }
}

void PostProcessing::setup(int width, int height) {
    destroy();

    m_width = width;
    m_height = height;

    m_quad_vao = OpenGL::init_quad();

    CHECKED_GL_CALL(glGenFramebuffers, 1, &m_fbo);
    CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, m_fbo);

    // generate texture
    CHECKED_GL_CALL(glGenTextures, 1, &m_texture_colorbuffer);
    CHECKED_GL_CALL(glBindTexture, GL_TEXTURE_2D, m_texture_colorbuffer);
    CHECKED_GL_CALL(glTexImage2D, GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    CHECKED_GL_CALL(glFramebufferTexture2D, GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture_colorbuffer, 0);

    // depth and stencil
    CHECKED_GL_CALL(glGenRenderbuffers, 1, &m_rbo);
    CHECKED_GL_CALL(glBindRenderbuffer, GL_RENDERBUFFER, m_rbo);
    CHECKED_GL_CALL(glRenderbufferStorage, GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    CHECKED_GL_CALL(glBindRenderbuffer, GL_RENDERBUFFER, 0);
    CHECKED_GL_CALL(glFramebufferRenderbuffer, GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rbo);

    int status = CHECKED_GL_CALL(glCheckFramebufferStatus, GL_FRAMEBUFFER);
    RG_GUARANTEE(status == GL_FRAMEBUFFER_COMPLETE, "Post processing not complete: {}", status);

    // back to default framebuffer
    CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, 0);
    CHECKED_GL_CALL(glBindTexture, GL_TEXTURE_2D, 0);
}

bool PostProcessing::needs_resize(int width, int height) const {
    return m_fbo == 0 || width != m_width || height != m_height;
}

void PostProcessing::begin() const {
    CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, m_fbo);
    OpenGL::enable_depth_testing();
}

void PostProcessing::end() const {
    CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, 0);
}

void PostProcessing::draw_quad(const resources::Shader *shader) const {
    // disable so that the fullscreen quad is always rendered
    OpenGL::disable_depth_testing();

    shader->use();
    CHECKED_GL_CALL(glActiveTexture, GL_TEXTURE0);
    CHECKED_GL_CALL(glBindTexture, GL_TEXTURE_2D, m_texture_colorbuffer);
    shader->set_int("screenTexture", 0);

    CHECKED_GL_CALL(glBindVertexArray, m_quad_vao);
    CHECKED_GL_CALL(glDrawArrays, GL_TRIANGLES, 0, 6);
    CHECKED_GL_CALL(glBindVertexArray, 0);
    CHECKED_GL_CALL(glBindTexture, GL_TEXTURE_2D, 0);
}

}// namespace engine::graphics
