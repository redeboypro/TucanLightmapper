//
// Created by redeb on 05.07.2024.
//

#include "Scene.h"

glm::vec4 Scene::lerp_rgba(const glm::vec4 &a, const glm::vec4 &b, const float t) {
    return {
        a.r + (b.r - a.r) * t,
        a.g + (b.g - a.g) * t,
        a.b + (b.b - a.b) * t,
        a.a + (b.a - a.a) * t
    };
}

Scene::Scene(Mesh *                              mesh,
             int32_t                             rays_per_texel,
             uint32_t                            width,
             uint32_t                            height,
             std::initializer_list<TexParameter> parameters) : m_lightmap_texture(width, height, parameters),
                                                               m_albedo_texture(width, height, parameters, COLOR_WHITE),
                                                               m_rays_per_texel(rays_per_texel) {
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
    auto &first_tri = m_triangles[m_tri];
    m_tri_tex_min   = m_lightmap_texture.to_pixel_coords(first_tri.tex_min);
    m_tri_tex_max   = m_lightmap_texture.to_pixel_coords(first_tri.tex_max);
}

void Scene::load_albedo_from_file(const std::string &file_name) {
    m_albedo_texture.load(file_name);
}

void Scene::bake_step() {
    static float cur_min_dist;
    if (m_iter >= m_iter_num) return;

    Triangle &  tri_a  = m_triangles[m_tri];
    const auto &normal = tri_a.a.normal;

    glm::vec3 ray_origin;
    glm::vec4 tmp_texel_col;

    for (int32_t y = m_tri_tex_min.y; y <= m_tri_tex_max.y; ++y) {
        for (int32_t       x        = m_tri_tex_min.x; x <= m_tri_tex_max.x; ++x) {
            if (const auto pixel_uv = m_lightmap_texture.to_uv_coords(x, y); tri_a.
                try_calculate_pt_from_uv(pixel_uv, ray_origin)) {
                glm::vec4 mul_col{};
                for (int32_t i = 0; i < m_rays_per_texel; ++i) {
                    auto ray_dir = normalize(glm::vec3{
                                                 random_floats(random_engine) * 2.0F - 1.0F,
                                                 random_floats(random_engine) * 2.0F - 1.0F,
                                                 random_floats(random_engine) * 2.0F - 1.0F
                                             });

                    if (dot(normal, ray_dir) < 0.0F) {
                        ray_dir = -ray_dir;
                    }

                    float form_factor      = 0.5F + 0.5F * dot(normal, ray_dir);
                    float min_raycast_dist = FLT_MAX;
                    bool  intersected      = false;

                    int32_t cur_tri = 0;
                    for (int32_t tri_bi = 0; tri_bi < m_triangles.size(); ++tri_bi) {
                        if (m_tri == tri_bi) continue;
                        if (Triangle &tri_b = m_triangles[tri_bi]; tri_b.try_raycast(
                             ray_origin, ray_dir, cur_min_dist)) {
                            intersected = true;
                            if (min_raycast_dist > cur_min_dist) {
                                min_raycast_dist = cur_min_dist;
                                cur_tri          = tri_bi;
                            }
                        }
                    }

                    if (intersected) {
                        Triangle &tri = m_triangles[cur_tri];
                        if (glm::vec2 res_uv{}; tri.try_calculate_uv_from_pt(
                                                                             ray_origin + min_raycast_dist * ray_dir,
                                                                             res_uv)) {
                            if (auto lightmap_pixel_coords = m_lightmap_texture.to_pixel_coords(res_uv);
                                m_lightmap_texture.get_pixel(lightmap_pixel_coords.x, lightmap_pixel_coords.y,
                                                             tmp_texel_col)) {
                                auto albedo_pixel_coords = m_albedo_texture.to_pixel_coords(res_uv);
                                if (glm::vec4 albedo_col; m_albedo_texture.get_pixel(
                                     albedo_pixel_coords.x, albedo_pixel_coords.y, albedo_col)) {
                                    mul_col += tmp_texel_col * lerp_rgba(glm::vec4{1.0F}, albedo_col, m_texel_power) *
                                            form_factor;
                                }
                            }
                        }
                    } else {
                        mul_col += glm::vec4{m_light_intensity * form_factor};
                    }
                }
                mul_col /= static_cast<float>(m_rays_per_texel);

                if (glm::vec4 src_col; m_lightmap_texture.get_pixel(x, y, src_col)) {
                    auto  result_col = clamp(src_col + mul_col, zero, one);
                    float denoise    = 1.0F;
                    if (m_lightmap_texture.get_pixel(x - 1, y, tmp_texel_col) && tmp_texel_col.a > 0.0F) {
                        result_col += tmp_texel_col;
                        denoise++;
                    }
                    if (m_lightmap_texture.get_pixel(x + 1, y, tmp_texel_col) && tmp_texel_col.a > 0.0F) {
                        result_col += tmp_texel_col;
                        denoise++;
                    }
                    if (m_lightmap_texture.get_pixel(x, y - 1, tmp_texel_col) && tmp_texel_col.a > 0.0F) {
                        result_col += tmp_texel_col;
                        denoise++;
                    }
                    if (m_lightmap_texture.get_pixel(x, y + 1, tmp_texel_col) && tmp_texel_col.a > 0.0F) {
                        result_col += tmp_texel_col;
                        denoise++;
                    }
                    result_col /= denoise;
                    result_col.a = 1.0F;
                    m_lightmap_texture.set_pixel(x, y, result_col);
                }
            }
        }
    }
    m_tri++;

    if (m_tri >= m_triangles.size()) {
        m_lightmap_texture.apply();
        m_tri = 0;
        m_iter++;
    }

    auto &cur_tri = m_triangles[m_tri];
    m_tri_tex_min = m_lightmap_texture.to_pixel_coords(cur_tri.tex_min);
    m_tri_tex_max = m_lightmap_texture.to_pixel_coords(cur_tri.tex_max);
}
