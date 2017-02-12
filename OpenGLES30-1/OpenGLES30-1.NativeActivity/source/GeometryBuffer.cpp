#include "GeometryBuffer.hpp"

#include "Vertex.hpp"

namespace gles30example
{
    GLuint createVbo(const Vertex *pVertices, int32_t sizeInBytes, GLenum mode)
    {
        GLuint vbo = 0;
        glGenBuffers(1, &vbo);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeInBytes, pVertices, mode);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        return vbo;
    }


    void releaseVbo(GLuint vbo)
    {
        glDeleteBuffers(1, &vbo);
    }
}