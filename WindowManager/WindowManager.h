#pragma once
#include "IWindow.h"
#include <vector>
#include <memory>
#include <unordered_map>
#include <string>

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

private:
    std::unordered_map<std::string, std::unique_ptr<IWindow>> windows_;
    bool running_;
};