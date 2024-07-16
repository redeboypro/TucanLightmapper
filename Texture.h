//
// Created by redeb on 05.07.2024.
//

#ifndef TEXTURE_H
#define TEXTURE_H
#include <format>
#include <vector>

#include "TucanGL.h"
#include "ThirdParty/lodepng.h"

#define COLOR_BLACK 0
#define COLOR_WHITE 255

struct TexParameter final {
    uint32_t name;
    int32_t  mode;
};

class Texture final {
    uint32_t             m_id = 0;
    std::vector<uint8_t> m_buffer, m_temp_buffer;
    uint32_t             m_width,  m_height;

    [[nodiscard]] bool try_get_pixel_index(const int32_t x, const int32_t y, uint32_t &index) const {
        index = (y * m_width + x) * 4;
        if (index >= m_buffer.size() - 3 or index < 0) {
            return false;
        }
        return true;
    }

    void store() const {
        glTexImage2D(
             GL_TEXTURE_2D,
             0,
             GL_RGBA8,
             static_cast<int32_t>(m_width),
             static_cast<int32_t>(m_height),
             0,
             GL_RGBA,
             GL_UNSIGNED_BYTE,
             &m_buffer[0]);
    }

    static uint8_t rgbau8(const float value) {
        return static_cast<uint8_t>(value * UINT8_MAX);
    }

    static float rgbaf(const uint8_t value) {
        return static_cast<float>(value) / UINT8_MAX;
    }
public:
    Texture(
        const uint32_t                      width,
        const uint32_t                      height,
        std::initializer_list<TexParameter> parameters,
        const uint8_t                       default_val = COLOR_BLACK) : m_buffer(width * height * 4, default_val),
                                                                         m_temp_buffer(width * height * 4, default_val),
                                                                         m_width(width),
                                                                         m_height(height) {
        glGenTextures(1, &m_id);
        bind();

        for (const auto [name, mode]: parameters) {
            glTexParameteri(GL_TEXTURE_2D, name, mode);
        }

        store();
    }

    ~Texture() {
        destroy();
    }

    const uint32_t &id = m_id;

    [[nodiscard]] uint32_t width() const {
        return m_width;
    }

    [[nodiscard]] uint32_t height() const {
        return m_height;
    }

    void bind() const {
        glBindTexture(GL_TEXTURE_2D, m_id);
    }

    bool get_pixel(
        const int32_t x,
        const int32_t y,
        glm::vec4 &   color) const {
        if (uint32_t index; try_get_pixel_index(x, y, index)) {
            color.r = rgbaf(m_buffer[index]);
            color.g = rgbaf(m_buffer[index + 1]);
            color.b = rgbaf(m_buffer[index + 2]);
            color.a = rgbaf(m_buffer[index + 3]);
            return true;
        }
        return false;
    }

    bool set_pixel(
        const int32_t    x,
        const int32_t    y,
        const glm::vec4 &color) {
        if (uint32_t index; try_get_pixel_index(x, y, index)) {
            m_temp_buffer[index]     = rgbau8(color.r);
            m_temp_buffer[index + 1] = rgbau8(color.g);
            m_temp_buffer[index + 2] = rgbau8(color.b);
            m_temp_buffer[index + 3] = rgbau8(color.a);
            return true;
        }
        return false;
    }

    void antialias(const int32_t x, const int32_t y, glm::vec4 &result_col, const float st_denoise = 0.0F) const {
        static glm::vec4 tmp_texel_col;
        static float denoise;

        denoise = st_denoise;
        if (get_pixel(x - 1, y, tmp_texel_col) && tmp_texel_col.a >
            0.0F) {
            result_col += tmp_texel_col;
            denoise++;
        }
        if (get_pixel(x + 1, y, tmp_texel_col) && tmp_texel_col.a >
            0.0F) {
            result_col += tmp_texel_col;
            denoise++;
        }
        if (get_pixel(x, y - 1, tmp_texel_col) && tmp_texel_col.a >
            0.0F) {
            result_col += tmp_texel_col;
            denoise++;
        }
        if (get_pixel(x, y + 1, tmp_texel_col) && tmp_texel_col.a >
            0.0F) {
            result_col += tmp_texel_col;
            denoise++;
        }
        if (denoise > 0.0F) {
            result_col /= denoise;
            result_col.a = 1.0F;
        }
    }

    void apply() {
        apply(0, 0, m_width, m_height);
    }

    void apply(
        const int32_t  xoffset, const int32_t yoffset,
        const uint32_t iwidth, const uint32_t iheight) {
        flush();
        if (uint32_t index; try_get_pixel_index(xoffset, yoffset, index)) {
            bind();
            glTexSubImage2D(
                            GL_TEXTURE_2D,
                            0,
                            xoffset, yoffset,
                            static_cast<int32_t>(iwidth),
                            static_cast<int32_t>(iheight),
                            GL_RGBA,
                            GL_UNSIGNED_BYTE,
                            &m_buffer[index]);
        }
    }

    void flush() {
        m_buffer = m_temp_buffer;
    }

    void destroy() const {
        glDeleteTextures(1, &m_id);
    }

    void load(const std::string &file_name) {
        m_temp_buffer.clear();
        lodepng::decode(m_temp_buffer, m_width, m_height, file_name);
        flush();
        store();
    }

    void save(const std::string &file_name) const {
        lodepng::encode(file_name, m_buffer, m_width, m_height);
    }

    [[nodiscard]] glm::vec2 to_uv_coords(const int32_t x, const int32_t y) const {
        return {
            (static_cast<float>(x) + 0.5F) / static_cast<float>(m_width),
            (static_cast<float>(y) + 0.5F) / static_cast<float>(m_height)
        };
    }

    [[nodiscard]] glm::ivec2 to_pixel_coords(const glm::vec2 &st) const {
        return {
            static_cast<int32_t>(st.s * static_cast<float>(m_width)),
            static_cast<int32_t>(st.t * static_cast<float>(m_height))
        };
    }

    Texture &operator=(const std::vector<uint8_t> &buffer) {
        m_buffer = buffer;
        apply();
        return *this;
    }

    static void active(const uint32_t texture) {
        glActiveTexture(33984 + texture);
    }
};

#endif //TEXTURE_H
