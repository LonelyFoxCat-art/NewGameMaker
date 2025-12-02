#pragma once
#include "IWindow.h"
#include "Win32Window.h"
#include <memory>
#include <string>

// 前向声明
class IRenderer;

// 渲染器类型枚举
enum class RendererType {
    DirectX,
    OpenGL,
    Vulkan,
    Software
};

class RenderWindow {
public:
    RenderWindow();
    ~RenderWindow();

    // 使用指定渲染器类型初始化窗口
    bool Initialize(const std::string& name, const std::string& title,
                   int x, int y, int width, int height, 
                   RendererType renderer_type);

    // 使用现有窗口和指定渲染器类型初始化
    bool InitializeWithExistingWindow(IWindow* window, RendererType renderer_type);

    // 执行渲染
    void Render();

    // 更新窗口
    void Update();

    // 显示/隐藏窗口
    void Show();
    void Hide();

    // 获取底层窗口
    IWindow* GetWindow() const;

    // 获取渲染器
    IRenderer* GetRenderer() const;

    // 获取/设置窗口名称
    std::string GetName() const;
    void SetName(const std::string& name);

    // 检查窗口是否有效
    bool IsValid() const;

private:
    IRenderer* CreateRenderer(RendererType type, IWindow* window);

    std::unique_ptr<Win32Window> window_;  // 底层窗口（如果拥有）
    IWindow* external_window_;             // 外部窗口（如果不拥有）
    IRenderer* renderer_;                  // 渲染器
    std::string name_;                     // 窗口名称
    bool owns_renderer_;                   // 是否拥有渲染器
};