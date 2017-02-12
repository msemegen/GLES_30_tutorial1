#pragma once 

#include <GLES3/gl3.h>
#include <vector>

#include "VertexAttribute.hpp"

namespace gles30example
{
    struct Vertex;
    struct Face;

    GLuint createVbo(const Vertex *pVertices, int32_t sizeInBytes, GLenum mode);

    void releaseVbo(GLuint vbo);
}