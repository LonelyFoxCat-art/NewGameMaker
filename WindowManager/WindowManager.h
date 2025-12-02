#pragma once
#include "IWindow.h"
#include <vector>
#include <memory>
#include <unordered_map>
#include <string>

// 表示矩形区域的结构体
struct Rect {
    int x, y, width, height;
    
    Rect(int x = 0, int y = 0, int width = 0, int height = 0) 
        : x(x), y(y), width(width), height(height) {}
    
    // 检查两个矩形是否相交
    bool Intersects(const Rect& other) const {
        return x < other.x + other.width &&
               x + width > other.x &&
               y < other.y + other.height &&
               y + height > other.y;
    }
    
    // 计算两个矩形的交集
    Rect GetIntersection(const Rect& other) const {
        int left = std::max(x, other.x);
        int top = std::max(y, other.y);
        int right = std::min(x + width, other.x + other.width);
        int bottom = std::min(y + height, other.y + other.height);
        
        if (left < right && top < bottom) {
            return Rect(left, top, right - left, bottom - top);
        }
        return Rect(0, 0, 0, 0); // 无交集
    }
};

class WindowManager {
public:
    WindowManager();
    ~WindowManager();

    // 创建窗口
    IWindow* CreateWindow(const std::string& name, const std::string& title, 
                         int x, int y, int width, int height);

    // 销毁窗口
    bool DestroyWindow(const std::string& name);
    bool DestroyWindow(IWindow* window);

    // 获取窗口
    IWindow* GetWindow(const std::string& name) const;

    // 更新所有窗口
    void UpdateAllWindows();

    // 显示/隐藏所有窗口
    void ShowAllWindows();
    void HideAllWindows();

    // 获取窗口数量
    size_t GetWindowCount() const;

    // 获取所有窗口名称
    std::vector<std::string> GetWindowNames() const;

    // 检查窗口是否存在
    bool HasWindow(const std::string& name) const;

    // 主循环处理
    void ProcessMessages();

    // 退出应用
    void Exit();

    // 窗口重叠处理相关
    void RenderAllWindows(); // 新增：渲染所有窗口，处理重叠边框

private:
    std::unordered_map<std::string, std::unique_ptr<IWindow>> windows_;
    bool running_;
    
    // 检查窗口是否与其他窗口重叠
    bool IsWindowOverlapping(IWindow* window) const;
    
    // 获取窗口的重叠区域
    std::vector<Rect> GetOverlappingAreas(IWindow* window) const;
};