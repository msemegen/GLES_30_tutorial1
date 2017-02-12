#pragma once

#include "Vector3.hpp"
#include "Color4.hpp"

namespace gles30example
{
    struct Vertex
    {
        Vector3 position;
        Color4 color;

        Vertex()
            : position()
            , color()
        {}

        Vertex(const Vector3 &position, const Color4 &color)
            : position(position)
            , color(color)
        {}
    };
}