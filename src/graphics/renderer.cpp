#include "renderer.h"
#include <math.h>
#include <memory>
#include "../debug.h"
#include "../maths/calc.h"

namespace ITD {

namespace {
    const std::string default_vert_str =
        "#version 330\n"
        "uniform mat4 u_matrix;\n"
        "layout(location=0) in vec3 a_position;\n"
        "layout(location=1) in vec2 a_uv;\n"
        "layout(location=2) in vec4 a_color;\n"
        "layout(location=3) in vec4 a_mask;\n"
        "out vec2 v_uv;\n"
        "out vec4 v_col;\n"
        "out vec4 v_mask;\n"
        "void main(void)\n"
        "{\n"
        "	gl_Position = u_matrix * vec4(a_position.xyz, 1);\n"
        "	v_uv = a_uv;\n"
        "	v_col = a_color;\n"
        "	v_mask = a_mask;\n"
        "}";

    const std::string default_frag_str =
        "#version 330\n"
        "uniform sampler2D u_texture;\n"
        "layout(location=0) out vec4 o_col;\n"
        "in vec2 v_uv;\n"
        "in vec4 v_col;\n"
        "in vec4 v_mask;\n"
        "void main(void)\n"
        "{\n"
        "   vec4 tcol = texture(u_texture, v_uv);\n"
        "   o_col = \n"
        "       tcol * v_col * v_mask.x + \n"
        "       tcol.a * v_col * v_mask.y + \n"
        "       v_col * v_mask.z;\n"
        "}";
}  // namespace

std::shared_ptr<Shader> Renderer::m_default_shader = nullptr;

Renderer::Renderer()
    : m_vertex_count(0)
    , m_vertex_map(nullptr)
    , m_index_map(nullptr)
    , m_matrix(glm::mat4(1.0f))
{
    // Create vertex array
    glGenVertexArrays(1, &m_vertex_array);
    glBindVertexArray(m_vertex_array);

    // Create vertex buffer
    glGenBuffers(1, &m_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);

    // Specify vertex buffer layout
    size_t pos_offset = 3 * sizeof(GLfloat);
    size_t uv_offset = 2 * sizeof(GLfloat);
    size_t color_offset = 4 * sizeof(GLubyte);
    size_t mask_offset = 4 * sizeof(GLubyte);

    GLsizei stride = pos_offset + uv_offset + color_offset + mask_offset;

    // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (const GLvoid *)0);

    // Texture uv
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride,
                          (const GLvoid *)pos_offset);

    // Color
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, stride,
                          (const GLvoid *)(pos_offset + uv_offset));

    // Mask
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(
        3, 3, GL_UNSIGNED_BYTE, GL_TRUE, stride,
        (const GLvoid *)(pos_offset + uv_offset + color_offset));

    // Allocate vertex buffer memory
    glBufferData(GL_ARRAY_BUFFER, RENDERER_MAX_VERTICES * sizeof(Vertex),
                 nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Create index buffer
    glGenBuffers(1, &m_index_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_buffer);

    // Allocate index buffer memory
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 RENDERER_MAX_INDICES * sizeof(GLushort), nullptr,
                 GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    if (!m_default_shader)
    {
        m_default_shader =
            std::make_shared<Shader>(default_vert_str, default_frag_str);
    }

    m_default_material.set_shader(m_default_shader.get());
    m_batch_front.count = 0;
    m_batch_front.texture = nullptr;
    m_batch_front.material = nullptr;
}

Renderer::~Renderer()
{
    glDeleteBuffers(1, &m_vertex_buffer);
    glDeleteBuffers(1, &m_index_buffer);
    glDeleteVertexArrays(1, &m_vertex_array);
}

void Renderer::make_vertex(float px, float py, float pz, float tx, float ty,
                           Color color, uint8_t mult, uint8_t wash,
                           uint8_t fill)
{
    m_vertex_map->pos.x = m_matrix[0][0] * px + m_matrix[1][0] * py +
                          m_matrix[2][0] * pz + m_matrix[3][0];

    m_vertex_map->pos.y = m_matrix[0][1] * px + m_matrix[1][1] * py +
                          m_matrix[2][1] * pz + m_matrix[3][1];

    m_vertex_map->pos.z = m_matrix[0][2] * px + m_matrix[1][2] * py +
                          m_matrix[2][2] * pz + m_matrix[3][2];

    m_vertex_map->uv.x = tx;
    m_vertex_map->uv.y = ty;
    m_vertex_map->color = color;
    m_vertex_map->mult = mult;
    m_vertex_map->wash = wash;
    m_vertex_map->fill = fill;
    m_vertex_map++;
}

void Renderer::push_triangle(float px0, float py0, float pz0, float px1,
                             float py1, float pz1, float px2, float py2,
                             float pz2, float tx0, float ty0, float tx1,
                             float ty1, float tx2, float ty2, Color c0,
                             Color c1, Color c2, uint8_t mult, uint8_t wash,
                             uint8_t fill)
{
    make_vertex(px0, py0, pz0, tx0, ty0, c0, mult, wash, fill);
    make_vertex(px1, py1, pz1, tx1, ty1, c1, mult, wash, fill);
    make_vertex(px2, py2, pz2, tx2, ty2, c2, mult, wash, fill);

    *m_index_map = m_vertex_count;
    m_index_map++;
    *m_index_map = m_vertex_count + 1;
    m_index_map++;
    *m_index_map = m_vertex_count + 2;
    m_index_map++;

    m_batch_front.count += 3;
    m_vertex_count += 3;
}

void Renderer::push_quad(float px0, float py0, float pz0, float px1, float py1,
                         float pz1, float px2, float py2, float pz2, float px3,
                         float py3, float pz3, float tx0, float ty0, float tx1,
                         float ty1, float tx2, float ty2, float tx3, float ty3,
                         Color c0, Color c1, Color c2, Color c3, uint8_t mult,
                         uint8_t wash, uint8_t fill)
{
    make_vertex(px0, py0, pz0, tx0, ty0, c0, mult, wash, fill);
    make_vertex(px1, py1, pz1, tx1, ty1, c1, mult, wash, fill);
    make_vertex(px2, py2, pz2, tx2, ty2, c2, mult, wash, fill);
    make_vertex(px3, py3, pz3, tx3, ty3, c3, mult, wash, fill);

    *m_index_map = m_vertex_count;
    m_index_map++;
    *m_index_map = m_vertex_count + 1;
    m_index_map++;
    *m_index_map = m_vertex_count + 2;
    m_index_map++;

    *m_index_map = m_vertex_count + 2;
    m_index_map++;
    *m_index_map = m_vertex_count + 3;
    m_index_map++;
    *m_index_map = m_vertex_count;
    m_index_map++;

    m_batch_front.count += 6;
    m_vertex_count += 4;
}

void Renderer::set_texture(const Texture *texture)
{
    if (m_batch_front.count > 0 && m_batch_front.texture != texture)
    {
        m_batches.push_back(m_batch_front);
        m_batch_front.count = 0;
    }

    m_batch_front.texture = texture;
}

void Renderer::push_material(Material *material)
{
    m_material_stack.push_back(m_batch_front.material);

    if (m_batch_front.count > 0 && m_batch_front.material != material)
    {
        m_batches.push_back(m_batch_front);
        m_batch_front.count = 0;
    }

    m_batch_front.material = material;
}

Material *Renderer::pop_material()
{
    ITD_ASSERT(m_material_stack.size() > 0,
               "Can't pop from empty material stack");

    Material *was = m_batch_front.material;
    Material *material = m_material_stack.back();

    if (m_batch_front.count > 0 && m_batch_front.material != material)
    {
        m_batches.push_back(m_batch_front);
        m_batch_front.count = 0;
    }

    m_batch_front.material = material;

    return was;
}

void Renderer::push_matrix(const glm::mat4 &matrix, bool absolute)
{
    m_matrix_stack.push_back(m_matrix);

    if (absolute)
    {
        m_matrix = matrix;
    }
    else
    {
        m_matrix = m_matrix * matrix;
    }
}

glm::mat4 Renderer::pop_matrix()
{
    ITD_ASSERT(m_matrix_stack.size() > 0, "Can't pop from empty matrix stack");

    glm::mat4 was = m_matrix;
    m_matrix = m_matrix_stack.back();
    m_matrix_stack.pop_back();

    return was;
}

glm::mat4 Renderer::peek_matrix()
{
    return m_matrix;
}

void Renderer::begin()
{
    glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
    m_vertex_map = (Vertex *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_buffer);
    m_index_map =
        (GLushort *)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Renderer::tri(const glm::vec2 &pos0, const glm::vec2 &pos1,
                   const glm::vec2 &pos2, Color color)
{
    ITD_ASSERT(m_vertex_map && m_index_map,
               "Render phase has not been started");

    push_triangle(pos0.x, pos0.y, 0.0f, pos1.x, pos1.y, 0.0f, pos2.x, pos2.y,
                  0.0f, 0, 0, 0, 0, 0, 0, color, color, color, 0, 0, 255);
}

void Renderer::tri(const glm::vec3 &pos0, const glm::vec3 &pos1,
                   const glm::vec3 &pos2, Color color)
{
    ITD_ASSERT(m_vertex_map && m_index_map,
               "Render phase has not been started");

    push_triangle(pos0.x, pos0.y, pos0.z, pos1.x, pos1.y, pos1.z, pos2.x,
                  pos2.y, pos2.z, 0, 0, 0, 0, 0, 0, color, color, color, 0, 0,
                  255);
}

void Renderer::rect(const Rectf &r, Color color)
{
    rect(r.bl, r.tr, color);
}

void Renderer::rect(const glm::vec2 &bl, const glm::vec2 &tr, Color color)
{
    ITD_ASSERT(m_vertex_map && m_index_map,
               "Render phase has not been started");

    push_quad(bl.x, bl.y, 0.0f, bl.x, tr.y, 0.0f, tr.x, tr.y, 0.0f, tr.x, bl.y,
              0.0f, 0, 0, 0, 0, 0, 0, 0, 0, color, color, color, color, 0, 0,
              255);
}

void Renderer::rect_line(const Rectf &r, float t, Color color)
{
    // Bottom
    rect(r.bl, glm::vec2(r.tr.x, r.bl.y + t), color);

    // Top
    rect(glm::vec2(r.bl.x, r.tr.y - t), r.tr, color);

    // Left
    rect(r.bl, glm::vec2(r.bl.x + t, r.tr.y), color);

    // Right
    rect(glm::vec2(r.tr.x - t, r.bl.y), r.tr, color);
}

void Renderer::quad(const Quadf &q, Color color)
{
    quad(q.a, q.b, q.c, q.d, color);
}

void Renderer::quad(const glm::vec2 &a, const glm::vec2 &b, const glm::vec2 &c,
                    const glm::vec2 &d, Color color)
{
    ITD_ASSERT(m_vertex_map && m_index_map,
               "Render phase has not been started");

    push_quad(a.x, a.y, 0.0f, b.x, b.y, 0.0f, c.x, c.y, 0.0f, d.x, d.y, 0.0f, 0,
              0, 0, 0, 0, 0, 0, 0, color, color, color, color, 0, 0, 255);
}

void Renderer::quad(const glm::vec3 &a, const glm::vec3 &b, const glm::vec3 &c,
                    const glm::vec3 &d, Color color)
{
    ITD_ASSERT(m_vertex_map && m_index_map,
               "Render phase has not been started");

    push_quad(a.x, a.y, a.z, b.x, b.y, b.z, c.x, c.y, c.z, d.x, d.y, d.z, 0, 0,
              0, 0, 0, 0, 0, 0, color, color, color, color, 0, 0, 255);
}

void Renderer::quad_line(const Quadf &q, float t, Color color)
{
    glm::vec2 center = q.center();

    for (int i = 0; i < 4; i++)
    {
        glm::vec2 p1 = q.values[i];
        glm::vec2 p2 = q.values[(i + 1) % 4];

        glm::vec2 norm = Calc::normalize(center - (p1 + p2) / 2.0f);
        quad(p1, p1 + t * norm, p2 + t * norm, p2, color);
    }
}

void Renderer::line(const glm::vec2 &start, const glm::vec2 &end, float t,
                    Color color)
{
    glm::vec2 dir = Calc::normalize(end - start);
    glm::vec2 norm = glm::vec2(-dir.y, dir.x);

    line(start, end, t, norm, color);
}

void Renderer::line(const glm::vec2 &start, const glm::vec2 &end, float t,
                    const glm::vec2 &t_dir, Color color)
{
    ITD_ASSERT(m_vertex_map && m_index_map,
               "Render phase has not been started");

    quad(start, start + t * t_dir, end + t * t_dir, end, color);
}

void Renderer::circ(const glm::vec2 &center, float radius, unsigned int steps,
                    Color color)
{
    float step_rad = Calc::TAU / (float)steps;

    float rad = 0.0f;
    for (size_t i = 0; i < steps; i++)
    {
        float next_rad = rad + step_rad;

        glm::vec2 start = center + (glm::vec2(cosf(rad), sinf(rad)) * radius);
        glm::vec2 end =
            center + (glm::vec2(cosf(next_rad), sinf(next_rad)) * radius);

        tri(center, start, end, color);

        rad = next_rad;
    }
}

void Renderer::tex(const Texture *texture, const glm::vec2 &pos, Color color)
{
    ITD_ASSERT(m_vertex_map && m_index_map,
               "Render phase has not been started");

    set_texture(texture);

    float mx = pos.x + texture->width() - 1.0f;
    float my = pos.y + texture->height() - 1.0f;

    push_quad(pos.x, pos.y, 0.0f, pos.x, my, 0.0f, mx, my, 0.0f, mx, pos.y,
              0.0f, 0, 1, 0, 0, 1, 0, 1, 1, color, color, color, color, 255, 0,
              0);
}

void Renderer::tex(const Subtexture &subtexture, const glm::vec2 &pos,
                   Color color)
{
    ITD_ASSERT(m_vertex_map && m_index_map,
               "Render phase has not been started");

    // TODO: Check if texture is set?
    set_texture(subtexture.get_texture());

    // TODO: Const ref return?
    Recti bounds = subtexture.get_bounds();

    float mx = pos.x + bounds.width() - 1.0f;
    float my = pos.y + bounds.height() - 1.0f;

    const std::array<glm::vec2, 4> &coords = subtexture.get_tex_coords();

    push_quad(pos.x, pos.y, 0.0f, pos.x, my, 0.0, mx, my, 0.0f, mx, pos.y, 0.0f,
              coords[0].x, 1.0f - coords[0].y, coords[1].x, 1.0f - coords[1].y,
              coords[2].x, 1.0f - coords[2].y, coords[3].x, 1.0f - coords[3].y,
              color, color, color, color, 255, 0, 0);
}

void Renderer::end()
{
    glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
    glUnmapBuffer(GL_ARRAY_BUFFER);
    m_vertex_map = nullptr;
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_buffer);
    glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
    m_index_map = nullptr;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Renderer::render(const glm::mat4 &matrix)
{
    ITD_ASSERT(!m_vertex_map && !m_index_map,
               "Render phase has not been ended");

    // Nothing to draw
    if (m_batches.size() == 0 && m_batch_front.count == 0)
        return;

    glBindVertexArray(m_vertex_array);

    glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_buffer);

    // Add current batch to list
    m_batches.push_back(m_batch_front);

    glUseProgram(m_default_shader->id());

    // Render batches
    size_t offset = 0;
    for (auto &batch : m_batches)
    {
        if (!batch.material)
        {
            batch.material = &m_default_material;
        }

        const Shader *shader = batch.material->shader();
        ITD_ASSERT(shader, "Material must have shader");

        glUseProgram(shader->id());

        // Set universal uniforms
        batch.material->set_texture("u_texture", batch.texture);
        batch.material->set_value("u_matrix", &matrix[0][0]);

        // Upload uniform values
        GLint texture_slot = 0;

        auto val_iter = batch.material->get_values().begin();

        for (const auto &uniform : shader->uniforms())
        {
            const GLint location = shader->uniform_location(uniform.name);

            if (uniform.type == GL_SAMPLER_2D)
            {
                const Texture *tex = batch.material->get_texture(texture_slot);

                glActiveTexture(GL_TEXTURE0 + texture_slot);

                // TODO: No need to check if a texture is valid if they are kept active
                // for the whole game
                if (tex && glIsTexture(tex->id()))
                {
                    glBindTexture(GL_TEXTURE_2D, tex->id());
                }
                else
                {
                    glBindTexture(GL_TEXTURE_2D, 0);
                }

                glUniform1i(location, texture_slot);
                texture_slot++;

                continue;
            }

            switch (uniform.type)
            {
                case GL_FLOAT:
                    glUniform1f(location, *val_iter);
                    val_iter++;
                    break;
                case GL_FLOAT_VEC2:
                    glUniform2fv(location, 2, &(*val_iter));
                    val_iter += 2;
                    break;
                case GL_FLOAT_MAT4:
                    glUniformMatrix4fv(location, 1, GL_FALSE, &(*val_iter));
                    val_iter += 16;
                    break;
            };
        }

        glDrawElements(GL_TRIANGLES, batch.count, GL_UNSIGNED_SHORT,
                       (void *)(offset * sizeof(GLushort)));

        offset += batch.count;
    }

    glUseProgram(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    m_vertex_count = 0;
    m_batch_front.count = 0;
    m_batches.clear();
}

}  // namespace ITD
