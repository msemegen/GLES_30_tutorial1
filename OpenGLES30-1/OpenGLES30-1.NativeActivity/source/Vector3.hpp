#pragma once

namespace gles30example
{
    struct Vector3
    {
        float x;
        float y;
        float z;

        Vector3()
            : x(0.0f)
            , y(0.0f)
            , z(0.0f)
        {}

        Vector3(float x, float y, float z)
            : x(x)
            , y(y)
            , z(z)
        {}
    };
}