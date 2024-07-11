//
// Created by redeb on 06.07.2024.
//

#ifndef VERTEX_H
#define VERTEX_H
#include <vec2.hpp>
#include <vec3.hpp>

struct Vertex final {
    glm::vec3 origin;
    glm::vec3 normal;
    glm::vec2 uv;

    Vertex(const glm::vec3& origin, const glm::vec3& normal, const glm::vec2& uv) : origin(origin), normal(normal), uv(uv) {}

    float& x = origin.x;
    float& y = origin.y;
    float& z = origin.z;

    float& u = uv.s;
    float& v = uv.t;
};

#endif //VERTEX_H
