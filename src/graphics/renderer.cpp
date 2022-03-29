#include "renderer.h"
#include <assert.h>
#include <math.h>
#include <memory>
#include "../maths/calc.h"

namespace ITD
{
    namespace
    {
        const std::string default_vert_str =
		"#version 330\n"
        "uniform mat4 u_matrix;\n"
		"layout(location=0) in vec2 a_position;\n"
		"layout(location=1) in vec2 a_uv;\n"
		"layout(location=2) in vec4 a_color;\n"
		"layout(location=3) in vec4 a_mask;\n"
		"out vec2 v_uv;\n"
		"out vec4 v_col;\n"
		"out vec4 v_mask;\n"
		"void main(void)\n"
		"{\n"
		"	gl_Position = u_matrix * vec4(a_position.xy, 0, 1);\n"
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
    }

    std::shared_ptr<Shader> Renderer::m_default_shader = nullptr;

    Renderer::Renderer()
        : m_vertex_count(0), m_vertex_map(nullptr), m_index_map(nullptr), m_matrix(glm::mat4(1.0f))
    {
        // Create vertex array
        glGenVertexArrays(1, &m_vertex_array);
        glBindVertexArray(m_vertex_array);

        // Create vertex buffer
        glGenBuffers(1, &m_vertex_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);

        // Specify vertex buffer layout
        const size_t pos_offset = 2 * sizeof(GLfloat);
        const size_t uv_offset = 2 * sizeof(GLfloat);
        const size_t color_offset = 4 * sizeof(GLubyte);
        const size_t mask_offset = 4 * sizeof(GLubyte);

        const GLsizei stride = pos_offset + uv_offset + color_offset + mask_offset;

        // Position
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (const GLvoid*)0);

        // Texture uv
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (const GLvoid*)pos_offset);

        // Color
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, stride,
                (const GLvoid*)(pos_offset + uv_offset));

        // Mask
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_UNSIGNED_BYTE, GL_TRUE, stride,
                (const GLvoid*)(pos_offset + uv_offset + color_offset));

        // Allocate vertex buffer memory
        glBufferData(GL_ARRAY_BUFFER, RENDERER_MAX_VERTICES * sizeof(Vertex), nullptr,
                GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // Create index buffer
        glGenBuffers(1, &m_index_buffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_buffer);

        // Allocate index buffer memory
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, RENDERER_MAX_INDICES * sizeof(GLushort),
                nullptr, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        glBindVertexArray(0);

        if (!m_default_shader)
        {
            m_default_shader = std::make_shared<Shader>(default_vert_str, default_frag_str);
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

    void Renderer::make_vertex(float px, float py, float tx, float ty, Color color,
            uint8_t mult, uint8_t wash, uint8_t fill)
    {
        m_vertex_map->pos.x = m_matrix[0][0] * px + 
            m_matrix[1][0] * py + m_matrix[2][0] + m_matrix[3][0];

        m_vertex_map->pos.y = m_matrix[0][1] * px + 
            m_matrix[1][1] * py + m_matrix[2][1] + m_matrix[3][1];

        m_vertex_map->uv.x = tx;
        m_vertex_map->uv.y = ty;
        m_vertex_map->color = color;
        m_vertex_map->mult = mult;
        m_vertex_map->wash = wash;
        m_vertex_map->fill = fill;
        m_vertex_map++;
    }


    void Renderer::push_triangle(float px0, float py0, float px1, float py1,
             float px2, float py2, float tx0,
             float ty0, float tx1, float ty1, float tx2, float ty2,
             Color c0, Color c1, Color c2,
             uint8_t mult, uint8_t wash, uint8_t fill)
    {
        make_vertex(px0, py0, tx0, ty0, c0, mult, wash, fill);
        make_vertex(px1, py1, tx1, ty1, c1, mult, wash, fill);
        make_vertex(px2, py2, tx2, ty2, c2, mult, wash, fill);

        *m_index_map = m_vertex_count;
        m_index_map++;
        *m_index_map = m_vertex_count + 1;
        m_index_map++;
        *m_index_map = m_vertex_count + 2;
        m_index_map++;

        m_batch_front.count += 3;
        m_vertex_count += 3;
    }

    void Renderer::push_quad(float px0, float py0, float px1, float py1,
            float px2, float py2, float px3, float py3, float tx0, float ty0,
            float tx1, float ty1, float tx2, float ty2, float tx3, float ty3,
            Color c0, Color c1, Color c2, Color c3,
            uint8_t mult, uint8_t wash, uint8_t fill)
    {
        make_vertex(px0, py0, tx0, ty0, c0, mult, wash, fill);
        make_vertex(px1, py1, tx1, ty1, c1, mult, wash, fill);
        make_vertex(px2, py2, tx2, ty2, c2, mult, wash, fill);
        make_vertex(px3, py3, tx3, ty3, c3, mult, wash, fill);

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

    Material* Renderer::pop_material()
    {
        assert(m_matrix_stack.size() > 0);

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

    void Renderer::push_matrix(const glm::mat4& matrix, const bool absolute)
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
        assert(m_matrix_stack.size() > 0);

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
        m_vertex_map = (Vertex*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_buffer);
        m_index_map = (GLushort*)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    void Renderer::tri(const glm::vec2& pos0, const glm::vec2& pos1, const glm::vec2& pos2,
            const Color color)
    {
        assert(m_vertex_map && m_index_map);

        push_triangle(pos0.x, pos0.y, pos1.x, pos1.y, pos2.x, pos2.y, 0, 0, 0, 0, 0, 0,
                color, color, color, 0, 0, 255);
    }

    void Renderer::rect(const glm::vec2& bl, const glm::vec2& tr, const Color color)
    {
        assert(m_vertex_map && m_index_map);

        push_quad(bl.x, bl.y, bl.x, tr.y, tr.x, tr.y, tr.x, bl.y, 
                0, 0, 0, 0, 0, 0, 0, 0, color, color, color, color,
                0, 0, 255);
    }

    void Renderer::quad(const glm::vec2& a, const glm::vec2& b, const glm::vec2& c,
            const glm::vec2& d, const Color color)
    {
        assert(m_vertex_map && m_index_map);

        push_quad(a.x, a.y, b.x, b.y, c.x, c.y, d.x, d.y, 0, 0, 0, 0, 0, 0, 0, 0,
                color, color, color, color, 0, 0, 255);
    }


    void Renderer::circ(const glm::vec2& center, const float radius, const unsigned int steps,
            const Color color)
    {
        float step_rad = (2 * M_PI) / (float)steps;

        float rad = 0.0f;
        for (size_t i = 0; i < steps; i++)
        {
            float next_rad = rad + step_rad;

            glm::vec2 start = center + (glm::vec2(cosf(rad), sinf(rad)) * radius);
            glm::vec2 end = center + (glm::vec2(cosf(next_rad), sinf(next_rad)) * radius);

            tri(center, start, end, color);

            rad = next_rad;
        }
    }

    void Renderer::tex(const Texture *texture, const glm::vec2& pos, const Color color)
    {
        assert(m_vertex_map && m_index_map);

        set_texture(texture);

        float w = texture->width();
        float h = texture->height();

        push_quad(pos.x, pos.y, pos.x, pos.y + h, pos.x + w, pos.y + h, pos.x + w, pos.y,
                0, 1, 0, 0, 1, 0, 1, 1, color, color, color, color, 255, 0, 0);
    }

    void Renderer::tex(const Subtexture &subtexture, const glm::vec2 &pos, const Color color)
    {
        assert(m_vertex_map && m_index_map);

        // TODO: Check if texture is set?
        set_texture(subtexture.get_texture());

        // TODO: Const ref return?
        Recti bounds = subtexture.get_bounds();

        const float w = bounds.width();
        const float h = bounds.height();

        const std::array<glm::vec2, 4> &coords = subtexture.get_tex_coords();

        push_quad(pos.x, pos.y, pos.x, pos.y + h, pos.x + w, pos.y + h, pos.x + w, pos.y,
                coords[0].x, 1.0f - coords[0].y, coords[1].x, 1.0f - coords[1].y, 
                coords[2].x, 1.0f - coords[2].y, coords[3].x, 1.0f - coords[3].y, 
                color, color, color, color, 255, 0, 0);
    }

    void Renderer::tex(const Subtexture &subtexture, const glm::vec2 &pos, const float rotation, const Color color)
    {
        const Recti bounds = subtexture.get_bounds();
        const float w = bounds.width();
        const float h = bounds.height();

        const glm::mat4 mat = Calc::rotate(rotation, pos + glm::vec2(w, h) / 2.0f);
        push_matrix(mat);
        tex(subtexture, pos, color);
        pop_matrix();
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

    void Renderer::render(const glm::mat4& matrix)
    {
        assert(!m_vertex_map && !m_index_map);

        // Nothing to draw
        if (m_batches.size() == 0 && m_batch_front.count == 0) return;

        glBindVertexArray(m_vertex_array);

        glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_buffer);

        // Add current batch to list
        m_batches.push_back(m_batch_front);

        glUseProgram(m_default_shader->id());

        // Render batches
        size_t offset = 0;
        for (auto& batch : m_batches)
        {
            if (!batch.material)
            {
                batch.material = &m_default_material;
            }

            const Shader *shader = batch.material->shader();
            assert(shader);

            glUseProgram(shader->id());

            // Set universal uniforms
            batch.material->set_texture("u_texture", batch.texture);
            batch.material->set_value("u_matrix", &matrix[0][0]);

            // Upload uniform values
            const auto& uniforms = shader->uniforms();
            GLint texture_slot = 0;

            auto val_iter = batch.material->get_values().begin();

            for (auto& uniform : uniforms)
            {
                const GLint location = shader->uniform_location(uniform.name);

                if (uniform.type == GL_SAMPLER_2D)
                {
                    const Texture *tex = batch.material->get_texture(texture_slot);

                    glActiveTexture(GL_TEXTURE0 + texture_slot);
                    if (tex)
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
                    (void*)(offset * sizeof(GLushort)));

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
}
