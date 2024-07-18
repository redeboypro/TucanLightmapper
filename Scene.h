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

#define I32(VALUE)            static_cast<int32_t>(VALUE)
#define LIGHTMAP_SIZE         255
#define ITER_NUM              14
#define INDIR_LIGHT_INTENSITY 1.0F
#define REFLECTIVITY          1.0F
#define ANTIALIAS_PASS_NUM    6
#define NEAR_CLIP             0.0001F
#define SKY_COLOR             0.22F, 0.69F, 0.87F, 1.0F

constexpr glm::vec4 zero = {0.0F, 0.0F, 0.0F, 0.0F};
constexpr glm::vec4 one  = {1.0F, 1.0F, 1.0F, 1.0F};

struct Patch final {
    int32_t    tri;
    glm::ivec2 pixel_coords;
    glm::vec3  normal;
    glm::vec3  world_coords;
};

class Scene final {
#ifdef _USE_EMBREE
    RTCDevice   m_embree_device;
    RTCScene    m_embree_scene;
    RTCGeometry m_embree_mesh;
#endif

    std::mt19937                          random_engine;
    std::uniform_real_distribution<float> random_floats;

    Mesh *m_mesh;

    Texture m_lightmap_texture;
    Texture m_albedo_texture;

    int32_t m_rays_per_texel;

    std::vector<Patch>    m_patches;

    std::vector<Triangle> m_triangles;

    int32_t m_iter     = 0;
    int32_t m_iter_num = ITER_NUM;

    float m_light_intensity = INDIR_LIGHT_INTENSITY;
    float m_texel_power     = REFLECTIVITY;

    [[nodiscard]] RTCRayHit embree_raycast(const glm::vec3 &ray_origin, const glm::vec3 &ray_dir) const;
    [[nodiscard]] static glm::vec4 lerp_rgba(const glm::vec4 &a, const glm::vec4 &b, float t);
public:
    Scene(
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
    void bake_step(const glm::vec3 &light_dir);
};


#endif //SCENE_H
