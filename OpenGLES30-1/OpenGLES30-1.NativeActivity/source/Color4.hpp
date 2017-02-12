#pragma once

namespace gles30example
{
    struct Color4
    {
        float r;
        float g;
        float b;
        float a;

        Color4()
            : r(0.0f)
            , g(0.0f)
            , b(0.0f)
            , a(0.0f)
        {}

        Color4(float r, float g, float b, float a)
            : r(r)
            , g(g)
            , b(b)
            , a(a)
        {}
    };
}