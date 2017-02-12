#pragma once

#include <string>
#include <GLES3/gl3.h>

#include "Vector3.hpp"
#include "Color4.hpp"

namespace gles30example
{
    GLuint compileShader(const std::string &source, GLenum type, std::string *pOutErrorMessage = nullptr);
    void releaseShader(GLuint shader);

    GLuint linkShaderProgram(GLuint vertexShader, GLuint fragmentShader, std::string *pOutErrorMessage = nullptr);
    void releaseShaderProgram(GLuint program);

    void useShaderProgram(GLuint program);
}