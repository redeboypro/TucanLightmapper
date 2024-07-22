//
// Created by redeb on 05.07.2024.
//

#ifndef SCENE_H
#define SCENE_H
#include <random>

#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "Triangle.h"

#include <embree4/rtcore.h>
#include <bits/stl_algo.h>

#define I32(VALUE)            static_cast<int32_t>(VALUE)
#define OCCLUDED(VALUE)       (VALUE != RTC_INVALID_GEOMETRY_ID)
#define LIGHTMAP_SIZE         255
#define ITER_NUM              12
#define AMBIENT_INTENSITY     0.5F
#define DIR_SAMPLES           64
#define SOFTNESS              0.3F
#define AO_RADIUS             1.0F
#define ANTIALIAS_PASS_NUM    3
#define NEAR_CLIP             0.01F

constexpr glm::vec4 zero = {0.0F, 0.0F, 0.0F, 0.0F};
constexpr glm::vec4 one  = {1.0F, 1.0F, 1.0F, 1.0F};

struct Patch final {
    glm::ivec2 pixel_coords;
    glm::vec3  normal;
    glm::vec3  world_coords;
};

class Scene final {
    RTCDevice   m_embree_device;
    RTCScene    m_embree_scene;
    RTCGeometry m_embree_mesh;

    std::mt19937                          random_engine;
    std::uniform_real_distribution<float> random_floats;

    Mesh *m_mesh;

    Texture m_lightmap_texture;
    Texture m_albedo_texture;

    int32_t m_rays_per_texel;

    glm::vec3 m_light_main_dir;

    std::vector<Patch>    m_patches;
    std::vector<Triangle> m_triangles;

    [[nodiscard]] glm::vec3 get_cos_hemisphere_sample(const glm::vec3& normal);
    [[nodiscard]] RTCRayHit embree_raycast(const glm::vec3 &ray_origin, const glm::vec3 &ray_dir, const float &tmin, const float &tmax) const;
    [[nodiscard]] static glm::vec3 get_perp_vec(const glm::vec3& u);
    [[nodiscard]] static glm::vec3 project_on_plane(const glm::vec3 &normal, const glm::vec3 &pt);
    [[nodiscard]] static glm::vec4 lerp_rgba(const glm::vec4 &a, const glm::vec4 &b, float t);
public:
    Scene(
        const glm::vec3 &                   light_dir,
        Mesh *                              mesh,
        int32_t                             rays_per_texel,
        uint32_t                            width      = LIGHTMAP_SIZE,
        uint32_t                            height     = LIGHTMAP_SIZE,
        std::initializer_list<TexParameter> parameters = {
            TexParameter{GL_TEXTURE_MIN_FILTER, GL_LINEAR},
            TexParameter{GL_TEXTURE_MAG_FILTER, GL_LINEAR},
            TexParameter{GL_TEXTURE_WRAP_S, GL_REPEAT},
            TexParameter{GL_TEXTURE_WRAP_T, GL_REPEAT}
        });

    ~Scene();

    const Texture &lightmap_texture = m_lightmap_texture;
    const Texture &albedo_texture   = m_albedo_texture;

    void load_albedo_from_file(const std::string &file_name);
    void bake();
};


#endif //SCENE_H
