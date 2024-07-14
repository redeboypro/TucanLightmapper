//
// Created by redeb on 07.07.2024.
//

#ifndef CAMERA_H
#define CAMERA_H
#include <mat4x4.hpp>
#include <vec3.hpp>
#include <ext/matrix_clip_space.hpp>
#include <ext/matrix_transform.hpp>

constexpr glm::vec3 unit_x = {1.0F, 0.0F, 0.0F};
constexpr glm::vec3 unit_y = {0.0F, 1.0F, 0.0F};
constexpr glm::vec3 unit_z = {0.0F, 0.0F, 1.0F};

class Camera final {
public:
    Camera(
        const float fov,
        const float aspect_ratio,
        const float near = 0.01F,
        const float far  = 100.0F)
        : location(0.0F),
          projection(glm::perspective(fov, aspect_ratio, near, far)),
          m_front(-unit_z),
          m_right(unit_x),
          m_up(unit_y),
          m_pitch(0.0F),
          m_yaw(0.0F) {
    }

    glm::vec3        location;
    const glm::vec3 &front = m_front;
    const glm::vec3 &right = m_right;
    const glm::vec3 &up    = m_up;

    const glm::mat4 projection;

    glm::mat4 view() {
        m_front.x = std::cos(m_pitch) * std::cos(m_yaw);
        m_front.x = std::sin(m_pitch);
        m_front.x = std::cos(m_pitch) * std::sin(m_yaw);
        m_front   = normalize(m_front);
        m_right   = normalize(cross(m_front, unit_y));
        m_up      = normalize(cross(m_right, m_front));
        return lookAt(location, location + m_front, m_up);
    }

private:
    glm::vec3 m_front, m_right, m_up;
    float     m_pitch, m_yaw;
};


#endif //CAMERA_H
