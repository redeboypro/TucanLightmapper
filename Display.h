//
// Created by redeb on 07.07.2024.
//

#ifndef DISPLAY_H
#define DISPLAY_H
#include <chrono>
#include "TucanGL.h"

#define WINAPI_WND_CLASSNAME "WND"

using WindowMessageCallback = LRESULT CALLBACK(HWND wndHandle, uint32_t msg, WPARAM wParam, LPARAM lParam);
using TimePoint             = std::chrono::time_point<std::chrono::system_clock>;

class Display final {
    HWND      m_hWND;
    HDC       m_hDC;
    HGLRC     m_hGLRC;
    bool      m_running = true;
    TimePoint m_previous_time;

    static WindowMessageCallback WndProc;

public:
    Display(
        LPCSTR  title,
        int32_t width,
        int32_t height,
        int32_t gl_major,
        int32_t gl_minor);

    ~Display();

    const int32_t width, height;
    const bool &  running = m_running;

    void begin(float &delta_time);

    void flush() const;

    static void vsync(bool state);
};


#endif //DISPLAY_H
