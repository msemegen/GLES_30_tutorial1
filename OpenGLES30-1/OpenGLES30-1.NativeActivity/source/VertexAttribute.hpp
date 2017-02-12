#pragma once

#include <cstdint>

namespace gles30example
{
    struct VertexAttribute
    {
        std::int32_t componentsCount;
        std::int32_t offsetInBytes;
        std::int32_t vertexSizeInBytes;

        GLenum type;

        VertexAttribute()
            : componentsCount(0)
            , offsetInBytes(0)
            , vertexSizeInBytes(0)
            , type(-1)
        {}

        VertexAttribute(std::int32_t componentsCount, std::int32_t offsetInBytes, std::int32_t vertexSizeInBytes, GLenum type)
            : componentsCount(componentsCount)
            , offsetInBytes(offsetInBytes)
            , vertexSizeInBytes(vertexSizeInBytes)
            , type(type)
        {}
    };

}