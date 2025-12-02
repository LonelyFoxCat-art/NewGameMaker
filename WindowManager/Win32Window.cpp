#include "Win32Window.h"
#include <windowsx.h>
#include <string>

// 静态成员定义
ATOM Win32Window::window_class_atom_ = 0;
const char* Win32Window::kWindowClassName = "GameEngineWindowClass";

Win32Window::Win32Window() : hwnd_(nullptr), visible_(false), valid_(false) {
}

Win32Window::~Win32Window() {
    if (valid_) {
        Destroy();
    }
}

bool Win32Window::Create(const std::string& title, int x, int y, int width, int height) {
    if (valid_) {
        return false; // 窗口已存在
    }

    title_ = title;

    RegisterWindowClass();

    // 计算窗口矩形（包括边框）
    RECT window_rect = { 0, 0, width, height };
    AdjustWindowRect(&window_rect, WS_OVERLAPPEDWINDOW, FALSE);

    int window_width = window_rect.right - window_rect.left;
    int window_height = window_rect.bottom - window_rect.top;

    hwnd_ = CreateWindowEx(
        0,                                    // 扩展样式
        kWindowClassName,                     // 窗口类名
        title.c_str(),                        // 窗口标题
        WS_OVERLAPPEDWINDOW,                  // 窗口样式
        x,                                    // X位置
        y,                                    // Y位置
        window_width,                         // 宽度
        window_height,                        // 高度
        nullptr,                              // 父窗口
        nullptr,                              // 菜单
        GetModuleHandle(nullptr),             // 实例句柄
        this                                  // 附加参数
    );

    if (!hwnd_) {
        return false;
    }

    SetWindowLongPtr(hwnd_, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
    valid_ = true;
    return true;
}

void Win32Window::Destroy() {
    if (valid_ && hwnd_) {
        DestroyWindow(hwnd_);
        hwnd_ = nullptr;
        valid_ = false;
        UnregisterWindowClass();
    }
}

void Win32Window::Show() {
    if (valid_ && hwnd_) {
        ShowWindow(hwnd_, SW_SHOW);
        visible_ = true;
    }
}

void Win32Window::Hide() {
    if (valid_ && hwnd_) {
        ShowWindow(hwnd_, SW_HIDE);
        visible_ = false;
    }
}

void Win32Window::Update() {
    if (valid_ && hwnd_) {
        UpdateWindow(hwnd_);
    }
}

bool Win32Window::IsVisible() const {
    return visible_;
}

bool Win32Window::IsValid() const {
    return valid_ && hwnd_ != nullptr;
}

void Win32Window::SetTitle(const std::string& title) {
    title_ = title;
    if (valid_ && hwnd_) {
        SetWindowText(hwnd_, title.c_str());
    }
}

std::string Win32Window::GetTitle() const {
    return title_;
}

void Win32Window::SetPosition(int x, int y) {
    if (valid_ && hwnd_) {
        SetWindowPos(hwnd_, nullptr, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
    }
}

void Win32Window::GetPosition(int& x, int& y) const {
    if (valid_ && hwnd_) {
        RECT rect;
        GetWindowRect(hwnd_, &rect);
        x = rect.left;
        y = rect.top;
    }
}

void Win32Window::SetSize(int width, int height) {
    if (valid_ && hwnd_) {
        RECT window_rect = { 0, 0, width, height };
        AdjustWindowRect(&window_rect, WS_OVERLAPPEDWINDOW, FALSE);

        int window_width = window_rect.right - window_rect.left;
        int window_height = window_rect.bottom - window_rect.top;

        SetWindowPos(hwnd_, nullptr, 0, 0, window_width, window_height, 
                     SWP_NOMOVE | SWP_NOZORDER);
    }
}

void Win32Window::GetSize(int& width, int& height) const {
    if (valid_ && hwnd_) {
        RECT client_rect;
        GetClientRect(hwnd_, &client_rect);
        width = client_rect.right - client_rect.left;
        height = client_rect.bottom - client_rect.top;
    }
}

void* Win32Window::GetNativeHandle() {
    return hwnd_;
}

void Win32Window::BeginRender() {
    // 在开始渲染前可能需要的准备工作
    // 例如：锁定渲染上下文等
}

void Win32Window::EndRender() {
    // 在结束渲染后可能需要的操作
    // 例如：交换缓冲区等
}

LRESULT CALLBACK Win32Window::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    Win32Window* window = nullptr;

    if (message == WM_CREATE) {
        CREATESTRUCT* create_struct = reinterpret_cast<CREATESTRUCT*>(lParam);
        window = static_cast<Win32Window*>(create_struct->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
    } else {
        window = reinterpret_cast<Win32Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    }

    if (window) {
        switch (message) {
            case WM_DESTROY:
                PostQuitMessage(0);
                return 0;
            case WM_SIZE:
                // 窗口大小改变
                return 0;
            case WM_PAINT:
                // 处理绘制消息
                {
                    PAINTSTRUCT ps;
                    HDC hdc = BeginPaint(hwnd, &ps);
                    EndPaint(hwnd, &ps);
                }
                return 0;
        }
    }

    return DefWindowProc(hwnd, message, wParam, lParam);
}

void Win32Window::RegisterWindowClass() {
    if (window_class_atom_ == 0) {
        WNDCLASSEX wcex = {};
        wcex.cbSize = sizeof(WNDCLASSEX);
        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = WndProc;
        wcex.hInstance = GetModuleHandle(nullptr);
        wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wcex.lpszClassName = kWindowClassName;

        window_class_atom_ = RegisterClassEx(&wcex);
    }
}

void Win32Window::UnregisterWindowClass() {
    if (window_class_atom_ != 0) {
        UnregisterClass(kWindowClassName, GetModuleHandle(nullptr));
        window_class_atom_ = 0;
    }
}