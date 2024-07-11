//
// Created by redeb on 05.07.2024.
//

#ifndef MESH_H
#define MESH_H
#include <vector>

#include "TucanGL.h"

#define GL_VERTEX_ATTRIB_ARRAY           0
#define GL_TEXTURE_COORD_ATTRIB_ARRAY    1

class Mesh final {
    std::vector<float>    m_vertices;
    std::vector<float>    m_uvs;
    std::vector<uint32_t> m_indices;
    VAO *                 m_vertex_array;
    VBO *                 m_vertex_buffer;
    VBO *                 m_uv_buffer;
    EBO *                 m_index_buffer;
    glm::vec3             m_min{FLT_MAX}, m_max{FLT_MIN};

public:
    Mesh() {
        m_vertex_array = new VAO();
        m_vertex_array->bind();
        m_vertex_buffer = new VBO();
        m_uv_buffer     = new VBO();
        m_index_buffer  = new EBO();
    }

    ~Mesh() {
        m_vertex_array->bind();
        delete m_vertex_buffer;
        delete m_uv_buffer;
        delete m_index_buffer;
        delete m_vertex_array;
    }

    const std::vector<float> &   vertices = m_vertices;
    const std::vector<float> &   uvs      = m_uvs;
    const std::vector<uint32_t> &indices  = m_indices;

    const glm::vec3 &min = m_min;
    const glm::vec3 &max = m_max;

    [[nodiscard]] VAO *get_vertex_array() const {
        return m_vertex_array;
    }

    void set_vertices(const std::vector<float> &vertices) {
        m_vertices = vertices;
        m_vertex_array->bind();
        if (m_vertex_buffer->store(vertices.data(), static_cast<int32_t>(vertices.size()))) {
            glVertexAttribPointer(GL_VERTEX_ATTRIB_ARRAY, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        }
        for (int32_t vi = 0; vi < vertices.size(); vi += 3) {
            const float &x = vertices[vi];
            const float &y = vertices[vi + 1];
            const float &z = vertices[vi + 2];

            m_min.x = std::min(x, m_min.x);
            m_min.y = std::min(y, m_min.y);
            m_min.z = std::min(z, m_min.z);

            m_max.x = std::max(x, m_max.x);
            m_max.y = std::max(y, m_max.y);
            m_max.z = std::max(z, m_max.z);
        }
    }

    void set_tex_coords(const std::vector<float> &uvs) {
        m_uvs = uvs;
        m_vertex_array->bind();
        if (m_uv_buffer->store(uvs.data(), static_cast<int32_t>(uvs.size()))) {
            glVertexAttribPointer(GL_TEXTURE_COORD_ATTRIB_ARRAY, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
        }
    }

    void set_indices(const std::vector<uint32_t> &indices) {
        m_indices = indices;
        m_vertex_array->bind();
        m_index_buffer->store(indices.data(), static_cast<int32_t>(indices.size()));
    }

    void draw() const {
        m_vertex_array->bind();

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        glDrawElements(GL_TRIANGLES, static_cast<int32_t>(indices.size()), GL_UNSIGNED_INT, nullptr);

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);

        glBindVertexArray(0);
    }
};

#endif //MESH_H
