//
// Created by redeb on 05.07.2024.
//

#ifndef TUCANGL_H
#define TUCANGL_H

#include <cstdint>
#include <windows.h>
#include "GL/gl.h"
#include <glm.hpp>

#pragma region [WINAPI_COMMON]
#define WINAPI_DUMMY_WND_CLASSNAME "DUMMY_WND"
#pragma endregion

#pragma region [ASSERTING]
#define GLMSG            L"GL"
#define WINAPIMSG        L"WINAPI"
#define ASSERT(API, MSG) MessageBoxW(nullptr, API L": " MSG, L"Error", MB_OK | MB_ICONEXCLAMATION)
#define TERMINATE        exit(EXIT_FAILURE)
#pragma endregion

#pragma region [GL_ARB_FLAGS]
#define WGL_CONTEXT_MAJOR_VERSION_ARB             0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB             0x2092
#define WGL_CONTEXT_PROFILE_MASK_ARB              0x9126
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB          0x00000001
#define WGL_DRAW_TO_WINDOW_ARB                    0x2001
#define WGL_ACCELERATION_ARB                      0x2003
#define WGL_SUPPORT_OPENGL_ARB                    0x2010
#define WGL_DOUBLE_BUFFER_ARB                     0x2011
#define WGL_PIXEL_TYPE_ARB                        0x2013
#define WGL_COLOR_BITS_ARB                        0x2014
#define WGL_DEPTH_BITS_ARB                        0x2022
#define WGL_STENCIL_BITS_ARB                      0x2023
#define WGL_FULL_ACCELERATION_ARB                 0x2027
#define WGL_TYPE_RGBA_ARB                         0x202B
#pragma endregion

#pragma region [GL_EXT_MACROS]
using GLenumEXT = enum : uint32_t {
    GL_VERTEX_SHADER        =                     0x8B31,
    GL_FRAGMENT_SHADER      =                     0x8B30,
    GL_ARRAY_BUFFER         =                     34962,
    GL_ELEMENT_ARRAY_BUFFER =                     34963,
    GL_STATIC_DRAW          =                     0x88E0,
    GL_STATIC_READ          =                     0x88E6,
    GL_STATIC_COPY          =                     0x88E6,
    GL_DYNAMIC_DRAW         =                     0x88E8,
    GL_DYNAMIC_READ         =                     0x88E9,
    GL_DYNAMIC_COPY         =                     0x88EA
};

using wglCreateContextAttribsARB_t = HGLRC WINAPI(HDC hdc, HGLRC hShareContext, const int32_t *attribList);
using wglChoosePixelFormatARB_t    = bool WINAPI(HDC hdc, const int32_t* piAttribIList, const float* pfAttribFList, uint32_t nMaxFormats, int32_t* piFormats, uint32_t *nNumFormats);
using wglSwapIntervalEXT_t         = int32_t WINAPI(int32_t interval);
using glActiveTexture_t            = void WINAPI(uint32_t texture);
using glGenVertexArrays_t          = void WINAPI(int32_t n, uint32_t* arrays);
using glDeleteVertexArrays_t       = void WINAPI(int32_t n, const uint32_t *arrays);
using glBindVertexArray_t          = void WINAPI(uint32_t array);
using glGenBuffers_t               = void WINAPI(int32_t n, uint32_t* buffers);
using glDeleteBuffers_t            = void WINAPI(int32_t n, const uint32_t* buffers);
using glBindBuffer_t               = void WINAPI(GLenum target, uint32_t buffer);
using glBufferData_t               = void WINAPI(GLenum target, ptrdiff_t size, const void* data, GLenum usage);
using glBufferSubData_t            = void WINAPI(GLenum target, ptrdiff_t offset, ptrdiff_t size, const void* data);
using glEnableVertexAttribArray_t  = void WINAPI(uint32_t index);
using glDisableVertexAttribArray_t = void WINAPI(uint32_t index);
using glVertexAttribPointer_t      = void WINAPI(uint32_t index, int32_t size, GLenum type, bool normalized, int32_t stride, const void* pointer);
using glCreateProgram_t            = uint32_t WINAPI();
using glAttachShader_t             = void WINAPI(uint32_t program, uint32_t shader);
using glDetachShader_t             = void WINAPI(uint32_t program, uint32_t shader);
using glLinkProgram_t              = void WINAPI(uint32_t program);
using glValidateProgram_t          = void WINAPI(uint32_t program);
using glUseProgram_t               = void WINAPI(uint32_t program);
using glDeleteShader_t             = void WINAPI(uint32_t shader);
using glDeleteProgram_t            = void WINAPI(uint32_t program);
using glBindAttribLocation_t       = void WINAPI(uint32_t program, uint32_t index, const char* name);
using glUniform1f_t                = void WINAPI(int32_t location, GLfloat v0);
using glUniform2f_t                = void WINAPI(int32_t location, GLfloat v0, GLfloat v1);
using glUniform3f_t                = void WINAPI(int32_t location, GLfloat v0, GLfloat v1, GLfloat v2);
using glUniform4f_t                = void WINAPI(int32_t location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
using glUniform2fv_t               = void WINAPI(int32_t location, int32_t count, const GLfloat* value);
using glUniform3fv_t               = void WINAPI(int32_t location, int32_t count, const GLfloat* value);
using glUniform1i_t                = void WINAPI(int32_t location, int32_t v0);
using glUniform2i_t                = void WINAPI(int32_t location, int32_t v0, int32_t v1);
using glUniform3i_t                = void WINAPI(int32_t location, int32_t v0, int32_t v1, int32_t v2);
using glUniform4i_t                = void WINAPI(int32_t location, int32_t v0, int32_t v1, int32_t v2, int32_t v3);
using glUniformMatrix4fv_t         = void WINAPI(int32_t location, int32_t count, bool transpose, const float* value);
using glGetUniformLocation_t       = int32_t WINAPI(uint32_t program, const char* name);
using glCreateShader_t             = uint32_t WINAPI(uint32_t type);
using glShaderSource_t             = void WINAPI(uint32_t shader, int32_t count, const char** string, const int32_t* length);
using glCompileShader_t            = void WINAPI(uint32_t shader);
using glGetShaderiv_t              = void WINAPI(uint32_t shader, GLenum pname, int32_t *params);
using glGetShaderInfoLog_t         = void WINAPI(uint32_t shader, int32_t maxLength, int32_t* length, char* infoLog);
#pragma endregion

#pragma region [GL_EXT_FRONT]
inline wglCreateContextAttribsARB_t* wglCreateContextAttribsARB;
inline wglChoosePixelFormatARB_t*    wglChoosePixelFormatARB;
inline wglSwapIntervalEXT_t*         wglSwapIntervalEXT;
inline glActiveTexture_t*            glActiveTexture;
inline glGenVertexArrays_t*          glGenVertexArrays;
inline glDeleteVertexArrays_t*       glDeleteVertexArrays;
inline glBindVertexArray_t*          glBindVertexArray;
inline glGenBuffers_t*               glGenBuffers;
inline glDeleteBuffers_t*            glDeleteBuffers;
inline glBindBuffer_t*               glBindBuffer;
inline glBufferData_t*               glBufferData;
inline glBufferSubData_t*            glBufferSubData;
inline glEnableVertexAttribArray_t*  glEnableVertexAttribArray;
inline glDisableVertexAttribArray_t* glDisableVertexAttribArray;
inline glVertexAttribPointer_t*      glVertexAttribPointer;
inline glCreateProgram_t*            glCreateProgram;
inline glAttachShader_t*             glAttachShader;
inline glDetachShader_t*             glDetachShader;
inline glLinkProgram_t*              glLinkProgram;
inline glValidateProgram_t*          glValidateProgram;
inline glUseProgram_t*               glUseProgram;
inline glDeleteShader_t*             glDeleteShader;
inline glDeleteProgram_t*            glDeleteProgram;
inline glBindAttribLocation_t*       glBindAttribLocation;
inline glUniform1f_t*                glUniform1f;
inline glUniform2f_t*                glUniform2f;
inline glUniform3f_t*                glUniform3f;
inline glUniform4f_t*                glUniform4f;
inline glUniform2fv_t*               glUniform2fv;
inline glUniform3fv_t*               glUniform3fv;
inline glUniform1i_t*                glUniform1i;
inline glUniform2i_t*                glUniform2i;
inline glUniform3i_t*                glUniform3i;
inline glUniform4i_t*                glUniform4i;
inline glUniformMatrix4fv_t*         glUniformMatrix4fv;
inline glGetUniformLocation_t*       glGetUniformLocation;
inline glCreateShader_t*             glCreateShader;
inline glShaderSource_t*             glShaderSource;
inline glCompileShader_t*            glCompileShader;
inline glGetShaderiv_t*              glGetShaderiv;
inline glGetShaderInfoLog_t*         glGetShaderInfoLog;

inline void gl_init_ext() {
    const WNDCLASSA dummy_wnd_class = {
        .style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
        .lpfnWndProc   = DefWindowProcA,
        .hInstance     = GetModuleHandle(nullptr),
        .lpszClassName = WINAPI_DUMMY_WND_CLASSNAME,
    };

    if (!RegisterClassA(&dummy_wnd_class)) {
        ASSERT(WINAPIMSG, L"Failed to register dummy class!");
        TERMINATE;
    }

    const auto dummy_hWND = CreateWindowExA(
        0,
        dummy_wnd_class.lpszClassName,
        nullptr,
        0,
        CW_USEDEFAULT, CW_USEDEFAULT,
        CW_USEDEFAULT, CW_USEDEFAULT,
        nullptr,
        nullptr,
        dummy_wnd_class.hInstance,
        nullptr);

    if (!dummy_hWND) {
        ASSERT(WINAPIMSG, L"Failed to create dummy window instance!");
        TERMINATE;
    }

    const auto dummy_hDC = GetDC(dummy_hWND);

    constexpr PIXELFORMATDESCRIPTOR pixel_format_descriptor = {
        .nSize        = sizeof(PIXELFORMATDESCRIPTOR),
        .nVersion     = 1,
        .dwFlags      = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        .iPixelType   = PFD_TYPE_RGBA,
        .cColorBits   = 32,
        .cAlphaBits   = 8,
        .cDepthBits   = 24,
        .cStencilBits = 8,
        .iLayerType   = PFD_MAIN_PLANE,
    };

    const int32_t pixel_format = ChoosePixelFormat(dummy_hDC, &pixel_format_descriptor);
    if (!pixel_format) {
        ASSERT(WINAPIMSG, L"Failed to choose the modern OpenGL pixel format!");
        TERMINATE;
    }

    if (!SetPixelFormat(dummy_hDC, pixel_format, &pixel_format_descriptor)) {
        ASSERT(WINAPIMSG, L"Failed to set the modern OpenGL pixel format!");
        TERMINATE;
    }

    const auto dummy_hGLRC = wglCreateContext(dummy_hDC);
    if (!dummy_hGLRC) {
        ASSERT(GLMSG, L"Failed to activate modern OpenGL pixel format!");
        TERMINATE;
    }

    if (!wglMakeCurrent(dummy_hDC, dummy_hGLRC)) {
        ASSERT(GLMSG, L"Failed to register dummy!");
        TERMINATE;
    }

    wglCreateContextAttribsARB = reinterpret_cast<wglCreateContextAttribsARB_t *>(wglGetProcAddress("wglCreateContextAttribsARB"));
    wglChoosePixelFormatARB    = reinterpret_cast<wglChoosePixelFormatARB_t *>(wglGetProcAddress("wglChoosePixelFormatARB"));
    wglSwapIntervalEXT         = reinterpret_cast<wglSwapIntervalEXT_t *>(wglGetProcAddress("wglSwapIntervalEXT"));
    glActiveTexture            = reinterpret_cast<glActiveTexture_t *>(wglGetProcAddress("glActiveTexture"));
    glGenVertexArrays          = reinterpret_cast<glGenVertexArrays_t *>(wglGetProcAddress("glGenVertexArrays"));
    glDeleteVertexArrays       = reinterpret_cast<glDeleteVertexArrays_t *>(wglGetProcAddress("glDeleteVertexArrays"));
    glBindVertexArray          = reinterpret_cast<glBindVertexArray_t *>(wglGetProcAddress("glBindVertexArray"));
    glGenBuffers               = reinterpret_cast<glGenBuffers_t *>(wglGetProcAddress("glGenBuffers"));
    glDeleteBuffers            = reinterpret_cast<glDeleteBuffers_t *>(wglGetProcAddress("glDeleteBuffers"));
    glBindBuffer               = reinterpret_cast<glBindBuffer_t *>(wglGetProcAddress("glBindBuffer"));
    glBufferData               = reinterpret_cast<glBufferData_t *>(wglGetProcAddress("glBufferData"));
    glBufferSubData            = reinterpret_cast<glBufferSubData_t *>(wglGetProcAddress("glBufferSubData"));
    glEnableVertexAttribArray  = reinterpret_cast<glEnableVertexAttribArray_t *>(wglGetProcAddress("glEnableVertexAttribArray"));
    glDisableVertexAttribArray = reinterpret_cast<glDisableVertexAttribArray_t *>(wglGetProcAddress("glDisableVertexAttribArray"));
    glVertexAttribPointer      = reinterpret_cast<glVertexAttribPointer_t *>(wglGetProcAddress("glVertexAttribPointer"));
    glCreateProgram            = reinterpret_cast<glCreateProgram_t *>(wglGetProcAddress("glCreateProgram"));
    glAttachShader             = reinterpret_cast<glAttachShader_t *>(wglGetProcAddress("glAttachShader"));
    glDetachShader             = reinterpret_cast<glDetachShader_t *>(wglGetProcAddress("glDetachShader"));
    glLinkProgram              = reinterpret_cast<glLinkProgram_t *>(wglGetProcAddress("glLinkProgram"));
    glValidateProgram          = reinterpret_cast<glValidateProgram_t *>(wglGetProcAddress("glValidateProgram"));
    glUseProgram               = reinterpret_cast<glUseProgram_t *>(wglGetProcAddress("glUseProgram"));
    glDeleteShader             = reinterpret_cast<glDeleteShader_t *>(wglGetProcAddress("glDeleteShader"));
    glDeleteProgram            = reinterpret_cast<glDeleteProgram_t *>(wglGetProcAddress("glDeleteProgram"));
    glBindAttribLocation       = reinterpret_cast<glBindAttribLocation_t *>(wglGetProcAddress("glBindAttribLocation"));
    glUniform1f                = reinterpret_cast<glUniform1f_t *>(wglGetProcAddress("glUniform1f"));
    glUniform2f                = reinterpret_cast<glUniform2f_t *>(wglGetProcAddress("glUniform2f"));
    glUniform3f                = reinterpret_cast<glUniform3f_t *>(wglGetProcAddress("glUniform3f"));
    glUniform4f                = reinterpret_cast<glUniform4f_t *>(wglGetProcAddress("glUniform4f"));
    glUniform2fv               = reinterpret_cast<glUniform2fv_t *>(wglGetProcAddress("glUniform2fv"));
    glUniform3fv               = reinterpret_cast<glUniform3fv_t *>(wglGetProcAddress("glUniform3fv"));
    glUniform1i                = reinterpret_cast<glUniform1i_t *>(wglGetProcAddress("glUniform1i"));
    glUniform2i                = reinterpret_cast<glUniform2i_t *>(wglGetProcAddress("glUniform2i"));
    glUniform3i                = reinterpret_cast<glUniform3i_t *>(wglGetProcAddress("glUniform3i"));
    glUniform4i                = reinterpret_cast<glUniform4i_t *>(wglGetProcAddress("glUniform4i"));
    glUniformMatrix4fv         = reinterpret_cast<glUniformMatrix4fv_t *>(wglGetProcAddress("glUniformMatrix4fv"));
    glGetUniformLocation       = reinterpret_cast<glGetUniformLocation_t *>(wglGetProcAddress("glGetUniformLocation"));
    glCreateShader             = reinterpret_cast<glCreateShader_t *>(wglGetProcAddress("glCreateShader"));
    glShaderSource             = reinterpret_cast<glShaderSource_t *>(wglGetProcAddress("glShaderSource"));
    glCompileShader            = reinterpret_cast<glCompileShader_t *>(wglGetProcAddress("glCompileShader"));
    glGetShaderiv              = reinterpret_cast<glGetShaderiv_t *>(wglGetProcAddress("glGetShaderiv"));
    glGetShaderInfoLog         = reinterpret_cast<glGetShaderInfoLog_t *>(wglGetProcAddress("glGetShaderInfoLog"));

    wglMakeCurrent(dummy_hDC, nullptr);
    wglDeleteContext(dummy_hGLRC);
    ReleaseDC(dummy_hWND, dummy_hDC);
    DestroyWindow(dummy_hWND);
}

inline HGLRC gl_init(HDC__* const hDC, const int32_t major, const int32_t minor) {
    const int32_t pixel_format_attribs[] = {
        WGL_DRAW_TO_WINDOW_ARB,     1,
        WGL_SUPPORT_OPENGL_ARB,     1,
        WGL_DOUBLE_BUFFER_ARB,      1,
        WGL_ACCELERATION_ARB,       WGL_FULL_ACCELERATION_ARB,
        WGL_PIXEL_TYPE_ARB,         WGL_TYPE_RGBA_ARB,
        WGL_COLOR_BITS_ARB,         32,
        WGL_DEPTH_BITS_ARB,         24,
        WGL_STENCIL_BITS_ARB,       8,
        0
    };

    int32_t  pixel_format;
    uint32_t formats_num;
    wglChoosePixelFormatARB(hDC, pixel_format_attribs, nullptr, 1, &pixel_format, &formats_num);
    if (!formats_num) {
        ASSERT(GLMSG, L"Failed to set the modern OpenGL pixel format!");
        TERMINATE;
    }

    PIXELFORMATDESCRIPTOR pixel_format_descriptor;
    DescribePixelFormat(hDC, pixel_format, sizeof(PIXELFORMATDESCRIPTOR), &pixel_format_descriptor);
    if (!SetPixelFormat(hDC, pixel_format, &pixel_format_descriptor)) {
        ASSERT(GLMSG, "Failed to set the modern OpenGL pixel format!");
        TERMINATE;
    }

    const int32_t gl_arb_attribs[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, major,
        WGL_CONTEXT_MINOR_VERSION_ARB, minor,
        WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0
    };

    const auto hGLRC = wglCreateContextAttribsARB(hDC, nullptr, gl_arb_attribs);
    if (!hGLRC) {
        ASSERT(GLMSG, "Failed to create the modern OpenGL context!");
        TERMINATE;
    }

    if (!wglMakeCurrent(hDC, hGLRC)) {
        ASSERT(GLMSG, "Failed to activate the modern OpenGL rendering context!");
        TERMINATE;
    }

    return hGLRC;
}
#pragma endregion

#pragma region [GL_ADD-ONS]
template<GLenumEXT BUFFER_TYPE>
class BufferObject final {
    uint32_t m_id {};
    bool     m_stored {};
public:
    BufferObject() {
        glGenBuffers(1, &m_id);
    }

    ~BufferObject() {
        destroy();
    }

    void destroy() const {
        glDeleteBuffers(1, &m_id);
    }

    void bind() const {
        glBindBuffer(type, m_id);
    }

    template<typename T>
    bool store(T* data, const GLsizei size) {
        bind();
        if (!m_stored) {
            glBufferData(type, size * sizeof(T), data, GL_STATIC_DRAW);
            m_stored = true;
            return true;
        }
        glBufferSubData(type, 0, size * sizeof(T), data);
        return false;
    }

    static constexpr GLenumEXT type = BUFFER_TYPE;
};

using VBO = BufferObject<GL_ARRAY_BUFFER>;
using EBO = BufferObject<GL_ELEMENT_ARRAY_BUFFER>;

class VAO final {
    GLuint m_id {};
public:
    VAO() {
        glGenVertexArrays(1, &m_id);
    }

    ~VAO() {
        destroy();
    }

    void bind() const {
        glBindVertexArray(m_id);
    }

    void destroy() const {
        glDeleteVertexArrays(1, &m_id);
    }
};
#pragma endregion
#endif //TUCANGL_H
