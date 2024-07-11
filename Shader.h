//
// Created by redeb on 05.07.2024.
//

#ifndef SHADER_H
#define SHADER_H
#include <iostream>
#include <string>

#include "TucanGL.h"
#include <gtc/type_ptr.hpp>

struct ShaderAttrib final {
    GLuint      location;
    std::string name;
};

class Shader final {
    uint32_t m_program_id;
    uint32_t m_vertex_shader_id, m_fragment_shader_id;

    static uint32_t load_shader_from_source(const std::string &source, const GLenumEXT type) {
        const uint32_t shader_id  = glCreateShader(type);
        const char *   source_str = source.c_str();

        glShaderSource(shader_id, 1, &source_str, nullptr);
        glCompileShader(shader_id);

        int32_t info_log_len;
        glGetShaderiv(shader_id, 0x8B84/*GL_INFO_LOG_LENGTH*/, &info_log_len);

        if (info_log_len > 0) {
            auto *info_log = new char[info_log_len + 1];
            glGetShaderInfoLog(shader_id, info_log_len, nullptr, info_log);
            std::cout << info_log << std::endl;
            delete[] info_log;
        }

        return shader_id;
    }

public:
    Shader(const std::string &vertex_shader_code, const std::string &fragment_shader_code, auto... attribs) {
        m_vertex_shader_id   = load_shader_from_source(vertex_shader_code, GL_VERTEX_SHADER);
        m_fragment_shader_id = load_shader_from_source(fragment_shader_code, GL_FRAGMENT_SHADER);

        m_program_id = glCreateProgram();

        glAttachShader(m_program_id, m_vertex_shader_id);
        glAttachShader(m_program_id, m_fragment_shader_id);

        (bind_attrib(attribs.location, attribs.name), ...);

        glLinkProgram(m_program_id);
        glValidateProgram(m_program_id);
    }

    ~Shader() {
        destroy();
    }

    void use() const {
        glUseProgram(m_program_id);
    }

    void bind_attrib(const uint32_t location, const std::string &name) const {
        glBindAttribLocation(m_program_id, location, name.c_str());
    }

    [[nodiscard]] int32_t get_uniform_location(const std::string &name) const {
        return glGetUniformLocation(m_program_id, name.c_str());
    }

    static void vec2(
        const int32_t    location,
        const glm::vec2 &vec) {
        glUniform2fv(location, 1, value_ptr(vec));
    }

    static void vec3(
        const int32_t    location,
        const glm::vec3 &vec) {
        glUniform3fv(location, 1, value_ptr(vec));
    }

    static void mat4(const int32_t location, const glm::mat4 &mat, const bool transpose = false) {
        glUniformMatrix4fv(location, 1, transpose, value_ptr(mat));
    }

    void destroy() const {
        glUseProgram(0);
        glDetachShader(m_program_id, m_vertex_shader_id);
        glDetachShader(m_program_id, m_fragment_shader_id);
        glDeleteShader(m_vertex_shader_id);
        glDeleteShader(m_fragment_shader_id);
        glDeleteProgram(m_program_id);
    }
};


#endif //SHADER_H
