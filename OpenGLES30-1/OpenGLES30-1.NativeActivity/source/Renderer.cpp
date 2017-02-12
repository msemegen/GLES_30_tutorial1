#include "Renderer.hpp"
#include "Color4.hpp"

namespace gles30example
{
    void clear(const Color4 &color, GLbitfield mask)
    {
        glClear(mask);
        glClearColor(color.r, color.g, color.b, color.a);
    }

    void drawTriangles(GLuint vbo, int32_t primitivesCount, const std::vector<VertexAttribute> &attributes)
    {
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

            for (int32_t i = 0; i < (int32_t)attributes.size(); i++)
            {
                glEnableVertexAttribArray(i);
                glVertexAttribPointer(i, attributes[i].componentsCount, attributes[i].type, GL_FALSE, attributes[i].vertexSizeInBytes, (void*)(attributes[i].offsetInBytes));
            }

            glDrawArrays(GL_TRIANGLES, 0, primitivesCount * 3);

            for (int32_t i = 0; i < (int32_t)attributes.size(); i++)
            {
                glDisableVertexAttribArray(i);
            }

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}
