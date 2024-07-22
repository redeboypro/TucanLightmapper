//
// Created by redeb on 06.07.2024.
//

#ifndef TRIANGLE_H
#define TRIANGLE_H
#include <cfloat>

#include "Vertex.h"

struct Triangle final {
    Triangle(const Vertex &a, const Vertex &b, const Vertex &c) : a(a), b(b), c(c),
                                                                  tex_min(std::min(a.u, std::min(b.u, c.u)),
                                                                          std::min(a.v, std::min(b.v, c.v))),
                                                                  tex_max(std::max(a.u, std::max(b.u, c.u)),
                                                                          std::max(a.v, std::max(b.v, c.v))) {
    }

    const Vertex    a,       b, c;
    const glm::vec2 tex_min, tex_max;

    bool try_raycast(const glm::vec3 &ray_origin, const glm::vec3 &ray_dir, float &min_dist) const {
        const auto &va = a.origin;
        const auto &vb = b.origin;
        const auto &vc = c.origin;
        const auto &n  = a.normal;
        const auto  ea = vb - va;
        const auto  eb = vc - vb;
        const auto  ec = va - vc;

        const float normal_dot_ray = dot(n, ray_dir);
        if (std::abs(normal_dot_ray) < FLT_EPSILON) return false;

        const float normal_dot_pt = -dot(n, va);
        min_dist                  = -(dot(n, ray_origin) + normal_dot_pt) / normal_dot_ray;
        if (min_dist < 0.0F) return false;

        const auto hit_pt = ray_origin + min_dist * ray_dir;

        auto hit_to_tri = hit_pt - va;
        auto e_perp     = cross(ea, hit_to_tri);
        if (dot(n, e_perp) < 0.0F) return false;

        hit_to_tri = hit_pt - vb;
        e_perp     = cross(eb, hit_to_tri);
        if (dot(n, e_perp) < 0.0F) return false;

        hit_to_tri = hit_pt - vc;
        e_perp     = cross(ec, hit_to_tri);
        return dot(n, e_perp) >= 0.0F;
    }

    bool try_calculate_pt_from_uv(const glm::vec2 &uv, glm::vec3 &pt) const {
        const auto &a_uv = a.uv;
        const auto &b_uv = b.uv;
        const auto &c_uv = c.uv;

        const float denom = area(a_uv, b_uv, c_uv);
        if (denom == 0.0F) return false;

        const float u = area(b_uv, c_uv, uv) / denom;
        if (u < 0) return false;

        const float v = area(c_uv, a_uv, uv) / denom;
        if (v < 0) return false;

        const float w = area(a_uv, b_uv, uv) / denom;
        if (w < 0) return false;

        pt = a.origin * u + b.origin * v + c.origin * w;
        return true;
    }

    constexpr const Vertex &operator[](const size_t index) const {
        constexpr const Vertex Triangle::*accessors[] = {
            &Triangle::a,
            &Triangle::b,
            &Triangle::c,
        };
        return this->*accessors[index];
    }

    [[nodiscard]] float area() const {
        return area(a.origin, b.origin, c.origin);
    }

private:
    static float area(const glm::vec2 &a, const glm::vec2 &b, const glm::vec2 &c) {
        const auto v1 = a - c;
        const auto v2 = b - c;
        return (v1.s * v2.t - v1.t * v2.s) * 0.5F;
    }
};

#endif //TRIANGLE_H
