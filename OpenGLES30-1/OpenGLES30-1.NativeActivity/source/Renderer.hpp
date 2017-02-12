#pragma once

#include <GLES3/gl3.h>
#include <vector>

#include "VertexAttribute.hpp"

namespace gles30example
{
    struct Color4;

    void clear(const Color4 &color, GLbitfield mask);
    void drawTriangles(GLuint vbo, int32_t primitivesCount, const std::vector<VertexAttribute> &attributes);
}