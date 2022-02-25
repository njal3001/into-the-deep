#include "renderer.h"
#include <assert.h>
#include <math.h>
#include <memory>

namespace Uboat
{
    namespace
    {
        const std::string default_vert_str =
		"#version 330\n"
        "uniform mat4 u_matrix;\n"
		"layout(location=0) in vec2 a_position;\n"
		"layout(location=1) in vec2 a_uv;\n"
		"layout(location=2) in vec4 a_color;\n"
		"out vec2 v_uv;\n"
		"out vec4 v_col;\n"
		"void main(void)\n"
		"{\n"
		"	gl_Position = u_matrix * vec4(a_position.xy, 0, 1);\n"
		"	v_uv = a_uv;\n"
		"	v_col = a_color;\n"
		"}";

        const std::string default_frag_str =
		"#version 330\n"
        "uniform sampler2D u_texture;\n"
        "layout(location=0) out vec4 o_col;\n"
		"in vec2 v_uv;\n"
		"in vec4 v_col;\n"
		"void main(void)\n"
		"{\n"
        "   o_col = v_col;\n"
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
        size_t pos_offset = 2 * sizeof(GLfloat);
        size_t uv_offset = 2 * sizeof(GLfloat);
        size_t color_offset = 4 * sizeof(GLubyte);

        GLsizei stride = pos_offset + uv_offset + color_offset;

        // position
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (const GLvoid*)0);

        // texture uv
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (const GLvoid*)pos_offset);

        // color
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, stride,
                (const GLvoid*)(pos_offset + uv_offset));

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

        m_matrix_stack.push_back(glm::mat3(1.0f));

        if (!m_default_shader)
        {
            m_default_shader = std::make_shared<Shader>(default_vert_str, default_frag_str);
        }
    }

    Renderer::~Renderer()
    {
        glDeleteBuffers(1, &m_vertex_buffer);
        glDeleteBuffers(1, &m_index_buffer);
        glDeleteVertexArrays(1, &m_vertex_array);
    }

    void Renderer::push_matrix(const glm::mat3& matrix, const bool absolute)
    {
        if (absolute)
        {
            m_matrix = matrix;
        }
        else
        {
            m_matrix = m_matrix * matrix;
        }

        m_matrix_stack.push_back(matrix);
    }

    glm::mat3 Renderer::pop_matrix()
    {
        assert(m_matrix_stack.size() > 1);

        glm::mat3 top = m_matrix_stack.back();
        m_matrix_stack.pop_back();
        m_matrix = m_matrix_stack.back();

        return top;
    }

    glm::mat3 Renderer::peek_matrix()
    {
        return m_matrix;
    }

    void Renderer::make_vertex(float px, float py, float tx, float ty, Color color)
    {
        m_vertex_map->pos.x = m_matrix[0][0] * px + m_matrix[1][0] * py + m_matrix[2][0];
        m_vertex_map->pos.y = m_matrix[0][1] * px + m_matrix[1][1] * py + m_matrix[2][1];
        m_vertex_map->uv.x = tx;
        m_vertex_map->uv.y = ty;
        m_vertex_map->color = color;

        m_vertex_map++;
    }


    void Renderer::update_batch(unsigned int count)
    {
        if (m_batches.empty())
        {
            m_batches.push_back({
                .count = 0
            });
        }

        m_batches.back().count += count;
    }

    void Renderer::push_triangle(float px0, float py0, float px1, float py1,
             float px2, float py2, unsigned int tex, float tx0,
             float ty0, float tx1, float ty1, float tx2, float ty2,
             Color c0, Color c1, Color c2)
    {
        make_vertex(px0, py0, tx0, ty0, c0);
        make_vertex(px1, py1, tx1, ty1, c1);
        make_vertex(px2, py2, tx2, ty2, c2);

        *m_index_map = m_vertex_count;
        m_index_map++;
        *m_index_map = m_vertex_count + 1;
        m_index_map++;
        *m_index_map = m_vertex_count + 2;
        m_index_map++;

        update_batch(3);
        m_vertex_count += 3;
    }

    void Renderer::push_quad(float px0, float py0, float px1, float py1,
            float px2, float py2, float px3, float py3,
            unsigned int tex, float tx0, float ty0, float tx1,
            float ty1, float tx2, float ty2, float tx3, float ty3,
            Color c0, Color c1, Color c2, Color c3)
    {
        make_vertex(px0, py0, tx0, ty0, c0);
        make_vertex(px1, py1, tx1, ty1, c1);
        make_vertex(px2, py2, tx2, ty2, c2);
        make_vertex(px3, py3, tx3, ty3, c3);

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

        update_batch(6);
        m_vertex_count += 4;
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

        push_triangle(pos0.x, pos0.y, pos1.x, pos1.y, pos2.x, pos2.y, 0, 0, 0, 0, 0, 0, 0,
                color, color, color);
    }

    void Renderer::rect(const glm::vec2& pos, const glm::vec2& size, const Color color)
    {
        assert(m_vertex_map && m_index_map);

        push_quad(pos.x, pos.y, pos.x, pos.y + size.y, pos.x + size.x, pos.y + size.y,
                pos.x + size.x, pos.y, 0, 0, 0, 0, 0, 0, 0, 0, 0, color, color, color, color);
    }

    void Renderer::quad(const glm::vec2& a, const glm::vec2& b, const glm::vec2& c,
            const glm::vec2& d, const Color color)
    {
        assert(m_vertex_map && m_index_map);

        push_quad(a.x, a.y, b.x, b.y, c.x, c.y, d.x, d.y, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                color, color, color, color);
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

        glBindVertexArray(m_vertex_array);

        glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_buffer);

        glUseProgram(m_default_shader->id());

        // Render batches
        size_t offset = 0;
        for (auto& batch : m_batches)
        {
            m_default_shader->set_uniform_mat4("u_matrix", matrix);

            glDrawElements(GL_TRIANGLES, batch.count, GL_UNSIGNED_SHORT,
                    (void*)(offset * sizeof(GLushort)));

            offset += batch.count;
        }

        glUseProgram(0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        m_vertex_count = 0;
        m_batches.clear();
    }
}
