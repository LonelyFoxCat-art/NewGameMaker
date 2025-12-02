#include "WindowManager.h"
#include "Win32Window.h"
#include <algorithm>

WindowManager::WindowManager() : running_(true) {
}

WindowManager::~WindowManager() {
    // 销毁所有窗口
    windows_.clear();
}

IWindow* WindowManager::CreateWindow(const std::string& name, const std::string& title, 
                                   int x, int y, int width, int height) {
    // 检查窗口是否已存在
    if (HasWindow(name)) {
        return nullptr;
    }

    // 创建新窗口
    auto window = std::make_unique<Win32Window>();
    if (!window->Create(title, x, y, width, height)) {
        return nullptr; // 创建失败
    }

    IWindow* window_ptr = window.get();
    windows_[name] = std::move(window);
    return window_ptr;
}

bool WindowManager::DestroyWindow(const std::string& name) {
    auto it = windows_.find(name);
    if (it != windows_.end()) {
        it->second->Destroy();
        windows_.erase(it);
        return true;
    }
    return false;
}

bool WindowManager::DestroyWindow(IWindow* window) {
    if (!window) {
        return false;
    }

    for (auto it = windows_.begin(); it != windows_.end(); ++it) {
        if (it->second.get() == window) {
            window->Destroy();
            windows_.erase(it);
            return true;
        }
    }
    return false;
}

IWindow* WindowManager::GetWindow(const std::string& name) const {
    auto it = windows_.find(name);
    if (it != windows_.end()) {
        return it->second.get();
    }
    return nullptr;
}

void WindowManager::UpdateAllWindows() {
    for (auto& pair : windows_) {
        pair.second->Update();
    }
}

void WindowManager::ShowAllWindows() {
    for (auto& pair : windows_) {
        pair.second->Show();
    }
}

void WindowManager::HideAllWindows() {
    for (auto& pair : windows_) {
        pair.second->Hide();
    }
}

size_t WindowManager::GetWindowCount() const {
    return windows_.size();
}

std::vector<std::string> WindowManager::GetWindowNames() const {
    std::vector<std::string> names;
    names.reserve(windows_.size());
    for (const auto& pair : windows_) {
        names.push_back(pair.first);
    }
    return names;
}

bool WindowManager::HasWindow(const std::string& name) const {
    return windows_.find(name) != windows_.end();
}

void WindowManager::ProcessMessages() {
#ifdef _WIN32
    MSG msg;
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) {
            running_ = false;
            break;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
#endif
}

void WindowManager::Exit() {
    running_ = false;
    // 销毁所有窗口
    for (auto& pair : windows_) {
        pair.second->Destroy();
    }
    windows_.clear();
}