#include <glad/glad.h>
#include <array>
#include <string>
#include <engine/graphics/OpenGL.hpp>
#include <engine/graphics/PointShadowMap.hpp>
#include <engine/resources/Shader.hpp>
#include <engine/util/Errors.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace engine::graphics {

void PointShadowMap::destroy() {
    if (m_depth_cubemap != 0) {
        CHECKED_GL_CALL(glDeleteTextures, 1, &m_depth_cubemap);
        m_depth_cubemap = 0;
    }
    if (m_fbo != 0) {
        CHECKED_GL_CALL(glDeleteFramebuffers, 1, &m_fbo);
        m_fbo = 0;
    }
}

void PointShadowMap::setup(int resolution) {
    destroy();
    m_resolution = resolution;

    CHECKED_GL_CALL(glGenFramebuffers, 1, &m_fbo);

    CHECKED_GL_CALL(glGenTextures, 1, &m_depth_cubemap);
    CHECKED_GL_CALL(glBindTexture, GL_TEXTURE_CUBE_MAP, m_depth_cubemap);
    for (int i = 0; i < 6; i++) {
        CHECKED_GL_CALL(glTexImage2D, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
                        (int) m_resolution, (int) m_resolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    }
    CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, m_fbo);
    CHECKED_GL_CALL(glFramebufferTexture, GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depth_cubemap, 0);
    CHECKED_GL_CALL(glDrawBuffer, GL_NONE);
    CHECKED_GL_CALL(glReadBuffer, GL_NONE);

    int status = CHECKED_GL_CALL(glCheckFramebufferStatus, GL_FRAMEBUFFER);
    RG_GUARANTEE(status == GL_FRAMEBUFFER_COMPLETE, "Point shadow map fb not complete: {}", status);

    CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, 0);
    CHECKED_GL_CALL(glBindTexture, GL_TEXTURE_CUBE_MAP, 0);
}

void PointShadowMap::begin(resources::Shader *shader, glm::vec3 &light_pos) {
    if (m_fbo == 0) {
        setup(m_resolution);
    }
    OpenGL::enable_depth_testing();
    CHECKED_GL_CALL(glViewport, 0, 0, (int) m_resolution, (int) m_resolution);
    CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, m_fbo);
    CHECKED_GL_CALL(glClear, GL_DEPTH_BUFFER_BIT);

    glm::mat4 shadow_proj = glm::perspective(glm::radians(90.0f), 1.0f, m_near_plane, m_far_plane);
    std::array<glm::mat4, 6> shadow_transforms = {
        shadow_proj * glm::lookAt(light_pos, light_pos + glm::vec3( 1, 0, 0), glm::vec3(0,-1, 0)),
        shadow_proj * glm::lookAt(light_pos, light_pos + glm::vec3(-1, 0, 0), glm::vec3(0,-1, 0)),
        shadow_proj * glm::lookAt(light_pos, light_pos + glm::vec3( 0, 1, 0), glm::vec3(0, 0, 1)),
        shadow_proj * glm::lookAt(light_pos, light_pos + glm::vec3( 0,-1, 0), glm::vec3(0, 0,-1)),
        shadow_proj * glm::lookAt(light_pos, light_pos + glm::vec3( 0, 0, 1), glm::vec3(0,-1, 0)),
        shadow_proj * glm::lookAt(light_pos, light_pos + glm::vec3( 0, 0,-1), glm::vec3(0,-1, 0)),
    };

    shader->use();
    for (int i = 0; i < 6; i++) {
        shader->set_mat4("shadow_matrices[" + std::to_string(i) + "]", shadow_transforms[i]);
    }
    shader->set_float("far_plane", m_far_plane);
    shader->set_vec3("light_pos", light_pos);
}

void PointShadowMap::end(int screen_width, int screen_height) {
    CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, 0);
    CHECKED_GL_CALL(glViewport, 0, 0, screen_width, screen_height);
}

} // engine::graphics namespace