//
// Created by redeb on 07.07.2024.
//

#include "Display.h"

LRESULT CALLBACK Display::WndProc(HWND hWND, const uint32_t msg, const WPARAM wParam, const LPARAM lParam) {
    switch (msg) {
        case WM_CLOSE: {
            PostQuitMessage(0);
            break;
        }
        default:
            return DefWindowProc(hWND, msg, wParam, lParam);
    }

    return 0;
}

Display::Display(const LPCSTR  title, const int32_t width, const int32_t height, const int32_t gl_major,
                 const int32_t gl_minor) : width(width), height(height) {
    gl_init_ext();
    const WNDCLASSEX wnd_class = {
        .cbSize = sizeof(WNDCLASSEX),
        .style = CS_OWNDC,
        .lpfnWndProc = &WndProc,
        .cbClsExtra = 0,
        .cbWndExtra = 0,
        .hInstance = GetModuleHandle(nullptr),
        .hIcon = LoadIcon(nullptr, IDI_APPLICATION),
        .hCursor = LoadCursor(nullptr, IDC_ARROW),
        .hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)),
        .lpszMenuName = nullptr,
        .lpszClassName = WINAPI_WND_CLASSNAME,
        .hIconSm = LoadIcon(nullptr, IDI_APPLICATION),
    };

    const auto wnd_class_reg_result = RegisterClassEx(&wnd_class);
    if (!wnd_class_reg_result) {
        ASSERT(WINAPIMSG, L"Failed to register window class!");
        TERMINATE;
    }

    RECT wnd_rect = {
        .left = 0,
        .top = 0,
        .right = width,
        .bottom = height
    };
    AdjustWindowRect(&wnd_rect, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU, false);

    m_hWND = CreateWindowEx(
                            0,
                            MAKEINTATOM(wnd_class_reg_result),
                            title,
                            WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT,
                            wnd_rect.right - wnd_rect.left, wnd_rect.bottom - wnd_rect.top,
                            nullptr,
                            nullptr,
                            nullptr,
                            nullptr);

    if (!m_hWND) {
        ASSERT(WINAPIMSG, L"Failed to create window instance!");
        TERMINATE;
    }

    m_hDC = GetDC(m_hWND);
    SetWindowLongPtr(m_hWND, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
    ShowWindow(m_hWND, SW_SHOW);
    UpdateWindow(m_hWND);

    m_hGLRC = gl_init(m_hDC, gl_major, gl_minor);
}

Display::~Display() {
    wglDeleteContext(m_hGLRC);
    DestroyWindow(m_hWND);
}

void Display::begin(float &delta_time) {
    MSG msg = {};
    if (PeekMessage(
                    &msg,
                    nullptr,
                    0,
                    0,
                    PM_REMOVE)) {
        if (msg.message != WM_QUIT) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        } else {
            m_running = false;
            return;
        }
    }

    const TimePoint current_time    = std::chrono::system_clock::now();
    auto            elapsed_seconds = std::chrono::duration<float>();

    if (m_previous_time.time_since_epoch().count())
        elapsed_seconds = current_time - m_previous_time;

    m_previous_time = current_time;
    delta_time      = elapsed_seconds.count();
}

void Display::flush() const {
    wglSwapLayerBuffers(m_hDC, WGL_SWAP_MAIN_PLANE);
}

void Display::vsync(const bool state) {
    wglSwapIntervalEXT(state);
}
