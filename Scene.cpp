//
// Created by redeb on 05.07.2024.
//

#include "Scene.h"

glm::vec3 Scene::get_perp_vec(const glm::vec3 &u) {
    const glm::vec3 a  = glm::abs(u);
    const uint32_t  xm = a.x - a.y < 0 && a.x - a.z < 0 ? 1 : 0;
    const uint32_t  ym = a.y - a.z < 0 ? 1 ^ xm : 0;
    const uint32_t  zm = 1 ^ (xm | ym);
    return cross(u, glm::vec3(xm, ym, zm));
}

glm::vec3 Scene::get_cos_hemisphere_sample(const glm::vec3 &normal) {
    const auto      rand  = glm::vec2{random_floats(random_engine), random_floats(random_engine)};
    const glm::vec3 bitan = get_perp_vec(normal);
    const glm::vec3 tan   = cross(bitan, normal);
    const float     r     = std::sqrt(rand.x);
    const float     phi   = 2.0F * 3.14159265F * rand.y;
    return tan * (r * glm::cos(phi)) + bitan * (r * glm::sin(phi)) + normal * glm::sqrt(1 - rand.x);
}

RTCRayHit Scene::embree_raycast(const glm::vec3 &ray_origin, const glm::vec3 &ray_dir, const float &tmin,
                                const float &    tmax) const {
    RTCRayHit ray_hit{};
    ray_hit.ray.org_x = ray_origin.x;
    ray_hit.ray.org_y = ray_origin.y;
    ray_hit.ray.org_z = ray_origin.z;

    ray_hit.ray.dir_x = ray_dir.x;
    ray_hit.ray.dir_y = ray_dir.y;
    ray_hit.ray.dir_z = ray_dir.z;

    ray_hit.ray.mask  = 0xFFFFFFFF;
    ray_hit.ray.tnear = tmin;
    ray_hit.ray.tfar  = tmax;

    ray_hit.hit.geomID = RTC_INVALID_GEOMETRY_ID;

    rtcIntersect1(m_embree_scene, &ray_hit);
    return ray_hit;
}

glm::vec3 Scene::project_on_plane(const glm::vec3 &normal, const glm::vec3 &pt) {
    return pt + dot(-pt, normal) * normal;
}

glm::vec4 Scene::lerp_rgba(const glm::vec4 &a, const glm::vec4 &b, const float t) {
    return {
        a.r + (b.r - a.r) * t,
        a.g + (b.g - a.g) * t,
        a.b + (b.b - a.b) * t,
        a.a + (b.a - a.a) * t
    };
}

Scene::Scene(const glm::vec3 &                   light_dir,
             Mesh *                              mesh,
             int32_t                             rays_per_texel,
             uint32_t                            width,
             uint32_t                            height,
             std::initializer_list<TexParameter> parameters) : m_lightmap_texture(width, height, parameters),
                                                               m_albedo_texture(width, height, parameters, COLOR_WHITE),
                                                               m_rays_per_texel(rays_per_texel),
                                                               m_light_main_dir(light_dir) {
    m_mesh = mesh;
    std::random_device random_device;
    random_engine = std::mt19937(random_device());
    random_floats = std::uniform_real_distribution(0.0F, 1.0F);

    const auto &indices  = mesh->indices;
    const auto &vertices = mesh->vertices;
    const auto &uvs      = mesh->uvs;

    for (int32_t i = 0; i < indices.size(); i += 3) {
        const auto a_ptr = I32(indices[i]);
        const auto b_ptr = I32(indices[i + 1]);
        const auto c_ptr = I32(indices[i + 2]);

        const auto a_v_ptr  = a_ptr * 3;
        const auto b_v_ptr  = b_ptr * 3;
        const auto c_v_ptr  = c_ptr * 3;
        const auto a_vt_ptr = a_ptr * 2;
        const auto b_vt_ptr = b_ptr * 2;
        const auto c_vt_ptr = c_ptr * 2;

        glm::vec3 a_v  = {vertices[a_v_ptr], vertices[a_v_ptr + 1], vertices[a_v_ptr + 2]};
        glm::vec3 a_vt = {uvs[a_vt_ptr], uvs[a_vt_ptr + 1], uvs[a_vt_ptr + 2]};

        glm::vec3 b_v  = {vertices[b_v_ptr], vertices[b_v_ptr + 1], vertices[b_v_ptr + 2]};
        glm::vec3 b_vt = {uvs[b_vt_ptr], uvs[b_vt_ptr + 1], uvs[b_vt_ptr + 2]};

        glm::vec3 c_v  = {vertices[c_v_ptr], vertices[c_v_ptr + 1], vertices[c_v_ptr + 2]};
        glm::vec3 c_vt = {uvs[c_vt_ptr], uvs[c_vt_ptr + 1], uvs[c_vt_ptr + 2]};

        glm::vec3 norm = normalize(cross(b_v - a_v, c_v - a_v));

        Vertex a = {a_v, norm, a_vt};
        Vertex b = {b_v, norm, b_vt};
        Vertex c = {c_v, norm, c_vt};

        m_triangles.emplace_back(a, b, c);
    }

    for (auto &tri: m_triangles) {
        auto tri_tex_min = m_lightmap_texture.to_pixel_coords(tri.tex_min);
        auto tri_tex_max = m_lightmap_texture.to_pixel_coords(tri.tex_max);
        for (int32_t y = tri_tex_min.y - 1; y <= tri_tex_max.y; ++y) {
            for (int32_t  x = tri_tex_min.x - 1; x <= tri_tex_max.x; ++x) {
                if (Patch patch{
                    .pixel_coords{x, y},
                    .normal = tri.a.normal,
                }; tri.try_calculate_pt_from_uv(m_lightmap_texture.to_uv_coords(x, y), patch.world_coords)) {
                    patch.world_coords += patch.normal * NEAR_CLIP;
                    m_patches.push_back(patch);
                }
            }
        }
    }
    m_embree_device = rtcNewDevice(nullptr);
    assert(m_embree_device && "Unable to create embree device.");

    m_embree_scene = rtcNewScene(m_embree_device);
    assert(m_embree_scene);

    m_embree_mesh = rtcNewGeometry(m_embree_device, RTC_GEOMETRY_TYPE_TRIANGLE);
    assert(mesh);

    auto *vertex_buffer = static_cast<float *>(rtcSetNewGeometryBuffer(m_embree_mesh, RTC_BUFFER_TYPE_VERTEX, 0,
                                                                       RTC_FORMAT_FLOAT3, 3 * sizeof(float),
                                                                       vertices.size() / 3));
    memcpy(vertex_buffer, vertices.data(), vertices.size() * sizeof(float));

    auto *index_buffer = static_cast<uint32_t *>(rtcSetNewGeometryBuffer(m_embree_mesh, RTC_BUFFER_TYPE_INDEX, 0,
                                                                         RTC_FORMAT_UINT3, 3 * sizeof(uint32_t),
                                                                         indices.size() / 3));
    memcpy(index_buffer, indices.data(), indices.size() * sizeof(uint32_t));

    rtcCommitGeometry(m_embree_mesh);
    rtcAttachGeometry(m_embree_scene, m_embree_mesh);
    rtcCommitScene(m_embree_scene);
}

Scene::~Scene() {
    rtcReleaseGeometry(m_embree_mesh);
    rtcReleaseScene(m_embree_scene);
    rtcReleaseDevice(m_embree_device);
}

void Scene::load_albedo_from_file(const std::string &file_name) {
    m_albedo_texture.load(file_name);
}

void Scene::bake() {
    static glm::vec4 result_col;
    const float smoothness = std::sin(glm::radians(SHADOW_ANGLE));
    for (int32_t iter = 0; iter < ITER_NUM; ++iter) {
        auto denom = static_cast<float>(iter);
        for (auto &[pixel_coords, normal, ray_origin]: m_patches) {
            float total_occlusion = 0.0F;
            for (int32_t ri = 0; ri < m_rays_per_texel; ri++) {
                auto ray_dir = normalize(get_cos_hemisphere_sample(normal));

                if (dot(ray_dir, normal) < 0.0F) {
                    ray_dir = -ray_dir;
                }

                if (auto [ray, hit] = embree_raycast(ray_origin, ray_dir, NEAR_CLIP, AO_RADIUS); OCCLUDED(hit.geomID)) {
                    total_occlusion++;
                }
            }
            total_occlusion = 1.0F - total_occlusion / static_cast<float>(m_rays_per_texel);
            float occlusion = 0.0F;
            for (int i = 0; i < DIR_SAMPLES; ++i) {
                glm::vec3 light_dir = m_light_main_dir + glm::vec3(
                                                                   (random_floats(random_engine) * 2.0F - 1.0F) * smoothness,
                                                                   random_floats(random_engine) * smoothness,
                                                                   (random_floats(random_engine) * 2.0F - 1.0F) * smoothness
                                                                  );
                light_dir = normalize(light_dir);

                if (OCCLUDED(embree_raycast(ray_origin, -light_dir, NEAR_CLIP, FLT_MAX).hit.geomID)) {
                    occlusion++;
                }
            }
            float diffuse       = std::max(dot(normal, -m_light_main_dir), 0.0F);
            float shadow_factor = 1.0F - occlusion / DIR_SAMPLES;
            total_occlusion *= shadow_factor * diffuse + AMBIENT_INTENSITY;
            auto delta_col = glm::vec4{total_occlusion, total_occlusion, total_occlusion, 1.0F};

            glm::vec4 total_col;
            m_lightmap_texture.get_pixel(pixel_coords.x, pixel_coords.y, total_col);
            m_lightmap_texture.set_pixel(pixel_coords.x, pixel_coords.y,
                                         clamp((denom * total_col + delta_col) / (denom + 1), zero, one));
        }
        m_lightmap_texture.apply();
    }

    for (int32_t pass = 0; pass < ANTIALIAS_PASS_NUM; ++pass) {
        for (int32_t y = 0; y < m_lightmap_texture.height(); ++y) {
            for (int32_t      x = 0; x < m_lightmap_texture.width(); ++x) {
                if (glm::vec4 base_col; m_lightmap_texture.get_pixel(x, y, base_col) &&
                                        base_col.a < 1.0F) {
                    result_col = {};
                    m_lightmap_texture.antialias(x, y, result_col, 0.0F);
                    m_lightmap_texture.set_pixel(x, y, result_col);
                }
            }
        }
        m_lightmap_texture.apply();
    }
    /*
     * m_lightmap_texture.save("path\\to\\output\\lightmap");
     */
}
