#include "android_native_app_glue.h"
#include <android/log.h>

#include <EGL/egl.h>
#include <GLES3/gl3.h>

#include "Vertex.hpp"
#include "Color4.hpp"

#include "GeometryBuffer.hpp"
#include "ShaderSystem.hpp"
#include "Renderer.hpp"

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "gles30example", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "gles30example", __VA_ARGS__))

namespace gles30example
{

    Vertex vertices[] = 
    {
        //       pozycja                //kolor             
        Vertex( {-0.5f, -0.25, -1.0f }, {1.0f, 1.0f, 1.0f, 1.0f} ),
        Vertex( {-0.5f, 0.25, -1.0f },  {1.0f, 1.0f, 1.0f, 1.0f} ),
        Vertex( {0.5f, 0.25, -1.0f },   {1.0f, 1.0f, 1.0f, 1.0f} ),
        Vertex( {0.5f, 0.25, -1.0f },   {1.0f, 1.0f, 1.0f, 1.0f} ),
        Vertex( {0.5f, -0.25, -1.0f },  {1.0f, 1.0f, 1.0f, 1.0f} ),
        Vertex( {-0.5f, -0.25, -1.0f }, {1.0f, 1.0f, 1.0f, 1.0f} )
    };

    struct AppData
    {
        EGLDisplay display;
        EGLSurface surface;
        EGLContext context;
        EGLConfig config;

        bool eglReady;
        bool shaderProgramLinked;
        bool correctOpenGLVersion;
    };

    struct QuadToRender
    {
        GLuint vbo;
        std::vector<VertexAttribute> veretxAttributes;
    };

    EGLDisplay initEglDisplay()
    {     
        EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

        if (EGL_NO_DISPLAY != display)
        {
            EGLint major = 0;
            EGLint minor = 0;
            EGLBoolean status = eglInitialize(display, &major, &minor);

            if (EGL_TRUE == status)
            {
                LOGI("EGL initialized with version %d.%d", major, minor);
            }
            else
            {
                LOGI("Cannot initialize EGL");
                display = EGL_NO_DISPLAY;
            }
        }

        return display;
    }

    EGLConfig findEglConfig(EGLDisplay display)
    {
        const EGLint configAttributes[] =
        {
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_RED_SIZE, 8,
            EGL_ALPHA_SIZE, 8,
            EGL_NONE
        };

        int32_t foundConfigsCount = 0;
        eglChooseConfig(display, configAttributes, nullptr, 0, &foundConfigsCount);

        EGLConfig *pConfigs = new EGLConfig[foundConfigsCount];

        eglChooseConfig(display, configAttributes, pConfigs, foundConfigsCount, &foundConfigsCount);

        EGLConfig foundConfig = nullptr;
        bool found = false;
        for (int32_t i = 0; i < foundConfigsCount && false == found; i++)
        {
            EGLint surface, r, g, b, a;
            found =
                EGL_TRUE == eglGetConfigAttrib(display, pConfigs[i], EGL_SURFACE_TYPE, &surface) &&
                EGL_TRUE == eglGetConfigAttrib(display, pConfigs[i], EGL_RED_SIZE, &r) &&
                EGL_TRUE == eglGetConfigAttrib(display, pConfigs[i], EGL_GREEN_SIZE, &g) &&
                EGL_TRUE == eglGetConfigAttrib(display, pConfigs[i], EGL_BLUE_SIZE, &b) &&
                EGL_TRUE == eglGetConfigAttrib(display, pConfigs[i], EGL_ALPHA_SIZE, &a) &&
                EGL_WINDOW_BIT & surface && 8 == r && 8 == g && 8 == b && 8 == a
            ;

            if (true == found)
            {
                foundConfig = pConfigs[i];
            }
        }

        delete[] pConfigs;

        return foundConfig;
    }

    EGLSurface createEglSurface(EGLDisplay display, EGLConfig config, ANativeWindow *pWindow)
    {
        return eglCreateWindowSurface(display, config, pWindow, nullptr);
    }

    EGLContext createEglContext(EGLDisplay display, EGLConfig config)
    {
        EGLint attributes[] = { EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE };
        return eglCreateContext(display, config, EGL_NO_CONTEXT, attributes);
    }

    void releaseEgl(EGLDisplay display, EGLSurface surface, EGLContext context)
    {
        if (EGL_NO_DISPLAY != display)
        {
            eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

            if (EGL_NO_CONTEXT != context)
            {
                eglDestroyContext(display, context);
            }

            if (EGL_NO_SURFACE != surface)
            {
                eglDestroySurface(display, surface);
            }

            eglTerminate(display);
        }
    }

    bool openglES30OrAboveCreated()
    {
        int32_t majorVersion;
        glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);

        return 3 <= majorVersion;
    }

    namespace
    {
        AppData appData;
        QuadToRender quadToRender;
        GLuint shaderProgram;

        std::string vertexShaderSource =
            "#version 300 es\n"
            "layout(location = 0) in vec3 attributePosition;\n"
            "layout(location = 1) in vec4 attributeColor;\n"
            "out vec4 vertexColor;"
            "void main(void)\n"
            "{\n"
            "   gl_Position = vec4(attributePosition, 1.0f);\n"
            "   vertexColor = attributeColor;"
            "}\n"
        ;

        std::string fragmentShaderSource =
            "#version 300 es\n"
            "out vec4 fragmentColor;\n"
            "in vec4 vertexColor;"
            "void main(void)\n"
            "{\n"
            "    fragmentColor = vertexColor;\n"
            "}\n"
        ;
    }
}


static void handleCmd(android_app *pAppState, int32_t cmd)
{
    switch (cmd)
    {
        case APP_CMD_GAINED_FOCUS:
        {
            using namespace gles30example;

            appData.eglReady = false;
            appData.shaderProgramLinked = false;
            appData.correctOpenGLVersion = false;

            if (nullptr != pAppState->window)
            {
                appData.display = initEglDisplay();

                if (EGL_NO_DISPLAY != appData.display)
                {
                    appData.config = findEglConfig(appData.display);

                    if (nullptr != appData.config)
                    {
                        appData.surface = createEglSurface(appData.display, appData.config, pAppState->window);

                        if (EGL_NO_SURFACE != appData.surface)
                        {
                            appData.context = createEglContext(appData.display, appData.config);

                            if (EGL_NO_CONTEXT != appData.context)
                            {
                                eglMakeCurrent(appData.display, appData.surface, appData.surface, appData.context);
                                appData.eglReady = true;
                            }
                        }
                    }
                }

                if (true == appData.eglReady)
                {
                    appData.correctOpenGLVersion = openglES30OrAboveCreated();

                    if (true == appData.correctOpenGLVersion)
                    {
                        quadToRender.vbo = createVbo(vertices, (int32_t)sizeof(vertices), GL_STATIC_DRAW);

                        std::string vertexShaderCompilationErrorMessage;
                        std::string fragmentShaderCompilationErrorMessage;

                        GLuint vertexShader = compileShader(vertexShaderSource, GL_VERTEX_SHADER, &vertexShaderCompilationErrorMessage);
                        GLuint fragmentShader = compileShader(fragmentShaderSource, GL_FRAGMENT_SHADER, &fragmentShaderCompilationErrorMessage);

                        if (0 != vertexShader && 0 != fragmentShader)
                        {
                            shaderProgram = linkShaderProgram(vertexShader, fragmentShader);
 
                            LOGI("Application ready to go");

                            appData.shaderProgramLinked = true;
                        }
                        else
                        {
                            LOGE("Vertex shader compilation error: %s", vertexShaderCompilationErrorMessage.c_str());
                            LOGE("Fragment shader compilation error: %s", fragmentShaderCompilationErrorMessage.c_str());
                        }
                    }
                }
                else
                {

                }
            }
        }
        break;

        case APP_CMD_LOST_FOCUS:
        {
            using namespace gles30example;

            if (true == appData.shaderProgramLinked)
            {
                releaseShader(shaderProgram);
            }

            if (true == appData.eglReady)
            {
                releaseVbo(quadToRender.vbo);
                releaseEgl(appData.display, appData.surface, appData.context);
            }

            LOGI("Window focus lost");
        }
        break;

        case APP_CMD_INIT_WINDOW:
        {
            LOGI("Window created");
        }
        break;

        case APP_CMD_TERM_WINDOW:
        {
            LOGI("Window released");
        }
        break;
    }
}

static int32_t handleInput(android_app *ppAppState, AInputEvent *pAppState)
{
    LOGI("Input detected");
    return 0;
}

void android_main(android_app *pAppState)
{
    using namespace gles30example;

    pAppState->onAppCmd = handleCmd;
    pAppState->onInputEvent = handleInput;

    quadToRender.veretxAttributes.push_back(VertexAttribute(3, 0, (int32_t)sizeof(Vertex), GL_FLOAT));
    quadToRender.veretxAttributes.push_back(VertexAttribute(4, offsetof(Vertex, color), (int32_t)sizeof(Vertex), GL_FLOAT));

    while (0 == pAppState->destroyed)
    {
        int32_t ret = 0;
        int32_t events = 0;
        android_poll_source *pPoolSource;

        while (0 <= (ret = ALooper_pollAll(0, nullptr, &events, (void**)&pPoolSource)))
        {
            if (nullptr != pPoolSource)
            {
                pPoolSource->process(pAppState, pPoolSource);
            }
        }

        if (true == appData.correctOpenGLVersion && true == appData.shaderProgramLinked && true == appData.eglReady)
        {
            clear(Color4(0.0f, 0.0f, 1.0f, 1.0f), GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            useShaderProgram(shaderProgram);
            drawTriangles(quadToRender.vbo, 2, quadToRender.veretxAttributes);
            useShaderProgram(0);

            eglSwapBuffers(appData.display, appData.surface);
        }
    }
}