#pragma once
#include <GL/glew.h>
#include <memory>
#include <unordered_map>
#include <vector>
#include "color.h"
#include "material.h"
#include "shader.h"
#include "subtexture.h"
#include "texture.h"

namespace ITD {

#define RENDERER_MAX_SPRITES 10000
#define RENDERER_MAX_VERTICES RENDERER_MAX_SPRITES * 4
#define RENDERER_MAX_INDICES RENDERER_MAX_SPRITES * 6

class Renderer
{
private:
    struct Vertex {
        glm::vec3 pos;
        glm::vec2 uv;
        Color color;
        uint8_t mult;
        uint8_t wash;
        uint8_t fill;
        uint8_t padd;
    };

    struct Batch {
        size_t count;
        const Texture *texture;
        Material *material;
    };

    GLuint m_vertex_array;
    GLuint m_vertex_buffer;
    GLuint m_index_buffer;

    size_t m_vertex_count;
    Vertex *m_vertex_map;
    GLushort *m_index_map;
    std::vector<Batch> m_batches;
    Batch m_batch_front;

    glm::mat4 m_matrix;
    std::vector<glm::mat4> m_matrix_stack;

    std::vector<Material *> m_material_stack;

    static std::shared_ptr<Shader> m_default_shader;
    Material m_default_material;

public:
    Renderer();
    ~Renderer();

    Renderer(const Renderer &other) = delete;
    Renderer &operator=(const Renderer &other) = delete;

    void set_texture(const Texture *texture);

    void push_material(Material *material);
    Material *pop_material();

    void push_matrix(const glm::mat4 &matrix, bool absolute = false);
    glm::mat4 pop_matrix();
    glm::mat4 peek_matrix();

    void begin();
    void render(const glm::mat4 &matrix);
    void end();

    void tri(const glm::vec2 &pos0, const glm::vec2 &pos1,
             const glm::vec2 &pos2, Color color);
    void tri(const glm::vec3 &pos0, const glm::vec3 &pos1,
             const glm::vec3 &pos2, Color color);

    void rect(const Rectf &r, Color color);
    void rect(const glm::vec2 &bl, const glm::vec2 &tr, Color color);
    void rect_line(const Rectf &r, float t, Color color);

    void quad(const Quadf &q, Color color);
    void quad(const glm::vec2 &a, const glm::vec2 &b, const glm::vec2 &c,
              const glm::vec2 &d, Color color);
    void quad(const glm::vec3 &a, const glm::vec3 &b, const glm::vec3 &c,
              const glm::vec3 &d, Color color);
    void quad_line(const Quadf &q, float t, Color color);

    void line(const glm::vec2 &start, const glm::vec2 &end, float t,
              Color color);

    void line(const glm::vec2 &start, const glm::vec2 &end, float t,
              const glm::vec2 &t_dir, Color color);

    void circ(const glm::vec2 &center, float radius, unsigned int steps,
              Color color);

    void tex(const Texture *texture, const glm::vec2 &pos, Color color);
    void tex(const Subtexture &subtexture, const glm::vec2 &pos, Color color);

private:
    void make_vertex(float px, float py, float pz, float tx, float ty,
                     Color color, uint8_t mult, uint8_t wash, uint8_t fill);

    void push_triangle(float px0, float py0, float pz0, float px1, float py1,
                       float pz1, float px2, float py2, float pz2, float tx0,
                       float ty0, float tx1, float ty1, float tx2, float ty2,
                       Color c0, Color c1, Color c2, uint8_t mult, uint8_t wash,
                       uint8_t fill);

    void push_quad(float px0, float py0, float pz0, float px1, float py1,
                   float pz1, float px2, float py2, float pz2, float px3,
                   float py3, float pz3, float tx0, float ty0, float tx1,
                   float ty1, float tx2, float ty2, float tx3, float ty3,
                   Color c0, Color c1, Color c2, Color c3, uint8_t mult,
                   uint8_t wash, uint8_t fill);
};

}  // namespace ITD
