#include "WindowManager.h"
#include "Win32Window.h"
#include <algorithm>
#include <iostream>

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

// 检查窗口是否与其他窗口重叠
bool WindowManager::IsWindowOverlapping(IWindow* window) const {
    if (!window) return false;
    
    Rect window_rect(window->GetX(), window->GetY(), window->GetWidth(), window->GetHeight());
    
    for (const auto& pair : windows_) {
        IWindow* other_window = pair.second.get();
        if (other_window != window && other_window->IsVisible()) {
            Rect other_rect(other_window->GetX(), other_window->GetY(), 
                           other_window->GetWidth(), other_window->GetHeight());
            
            if (window_rect.Intersects(other_rect)) {
                return true;
            }
        }
    }
    return false;
}

// 获取窗口的重叠区域
std::vector<Rect> WindowManager::GetOverlappingAreas(IWindow* window) const {
    std::vector<Rect> overlapping_areas;
    if (!window) return overlapping_areas;
    
    Rect window_rect(window->GetX(), window->GetY(), window->GetWidth(), window->GetHeight());
    
    for (const auto& pair : windows_) {
        IWindow* other_window = pair.second.get();
        if (other_window != window && other_window->IsVisible()) {
            Rect other_rect(other_window->GetX(), other_window->GetY(), 
                           other_window->GetWidth(), other_window->GetHeight());
            
            Rect intersection = window_rect.GetIntersection(other_rect);
            if (intersection.width > 0 && intersection.height > 0) {
                overlapping_areas.push_back(intersection);
            }
        }
    }
    
    // 合并重叠区域（如果它们相交）
    std::vector<Rect> merged_areas;
    for (const auto& area : overlapping_areas) {
        bool merged = false;
        for (auto& existing : merged_areas) {
            if (area.Intersects(existing)) {
                // 合并矩形 - 取包围盒
                int left = std::min(area.x, existing.x);
                int top = std::min(area.y, existing.y);
                int right = std::max(area.x + area.width, existing.x + existing.width);
                int bottom = std::max(area.y + area.height, existing.y + existing.height);
                existing = Rect(left, top, right - left, bottom - top);
                merged = true;
                break;
            }
        }
        if (!merged) {
            merged_areas.push_back(area);
        }
    }
    
    return merged_areas;
}

// 渲染所有窗口，处理重叠边框
void WindowManager::RenderAllWindows() {
    // 获取所有窗口并按Z顺序排序（从后往前）
    std::vector<IWindow*> visible_windows;
    for (const auto& pair : windows_) {
        if (pair.second->IsVisible()) {
            visible_windows.push_back(pair.second.get());
        }
    }
    
    // 按照Z顺序排序（这里简单按名称排序，实际应用中应使用窗口的Z顺序）
    std::sort(visible_windows.begin(), visible_windows.end(), 
              [](IWindow* a, IWindow* b) {
                  // 按照窗口位置排序，左上角坐标较小的在前面
                  if (a->GetY() != b->GetY()) {
                      return a->GetY() < b->GetY();
                  }
                  return a->GetX() < b->GetX();
              });
    
    // 渲染每个窗口
    for (IWindow* window : visible_windows) {
        // 开始渲染
        window->BeginRender();
        
        // 获取重叠区域
        std::vector<Rect> overlapping_areas = GetOverlappingAreas(window);
        
        // 根据重叠情况调整渲染（这里只是概念性实现，实际的边框渲染取决于具体图形API）
        if (!overlapping_areas.empty()) {
            // 如果窗口与其他窗口重叠，则可能需要特殊处理边框
            // 例如：不绘制重叠部分的边框
            std::cout << "Window at (" << window->GetX() << ", " << window->GetY() 
                      << ") has overlapping areas, adjusting border rendering..." << std::endl;
        }
        
        // 结束渲染
        window->EndRender();
    }
}