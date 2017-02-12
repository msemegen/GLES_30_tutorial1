#include "ShaderSystem.hpp"

namespace gles30example
{

    namespace
    {
        std::string getShaderCompilationErrorMessage(GLuint shader)
        {
            GLint errorStringLength;
            std::string message;

            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &errorStringLength);

            if (0 < errorStringLength)
            {
                char *pTempBuffer = new char[errorStringLength + 1];
                glGetShaderInfoLog(shader, errorStringLength, &errorStringLength, pTempBuffer);
                message = pTempBuffer;

                delete[] pTempBuffer;
            }

            return message;
        }

        std::string getProgramLinkingErrorMessage(GLuint program)
        {
            GLint errorStringLength;
            std::string message;

            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &errorStringLength);

            if (0 < errorStringLength)
            {
                char *pTempBuffer = new char[errorStringLength + 1];
                glGetProgramInfoLog(program, errorStringLength, &errorStringLength, pTempBuffer);
                message = pTempBuffer;

                delete[] pTempBuffer;
            }

            return message;
        }
    }

    GLuint compileShader(const std::string &source, GLenum type, std::string *pOutErrorMessage)
    {
        GLuint shader = glCreateShader(type);

        const char *pSource = source.c_str();
        glShaderSource(shader, 1, (const GLchar* const*)&(pSource), nullptr);
        glCompileShader(shader);

        GLint status = GL_FALSE;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

        if (GL_FALSE == status)
        {
            if (nullptr != pOutErrorMessage)
            {
                *pOutErrorMessage = getShaderCompilationErrorMessage(shader);
            }

            glDeleteShader(shader);
            shader = 0;
        }

        return shader;
    }

    void releaseShader(GLuint shader)
    {
        glDeleteShader(shader);
    }

    GLuint linkShaderProgram(GLuint vertexShader, GLuint fragmentShader, std::string *pOutErrorMessage)
    {
        GLuint program = 0;

        program = glCreateProgram();

        glAttachShader(program, vertexShader);
        glAttachShader(program, fragmentShader);

        glLinkProgram(program);

        GLint status = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &status);

        if (GL_FALSE == status)
        {
            if (nullptr != pOutErrorMessage)
            {
                *pOutErrorMessage = getProgramLinkingErrorMessage(program);
            }

            glDeleteProgram(program);
            program = 0;
        }

        return program;
    }

    void releaseShaderProgram(GLuint program)
    {
        glDeleteProgram(program);
    }

    void useShaderProgram(GLuint program)
    {
        glUseProgram(program);
    }
}