#include "RenderWindow.h"
#include "../Renderer/IRenderer.h"
#include <memory>

RenderWindow::RenderWindow() : window_(nullptr), renderer_(nullptr), owns_renderer_(false) {
}

RenderWindow::~RenderWindow() {
    if (owns_renderer_ && renderer_) {
        delete renderer_;
        renderer_ = nullptr;
    }
}

bool RenderWindow::Initialize(const std::string& name, const std::string& title,
                            int x, int y, int width, int height, 
                            RendererType renderer_type) {
    // 创建底层窗口
    window_ = std::make_unique<Win32Window>();
    if (!window_->Create(title, x, y, width, height)) {
        return false;
    }

    // 创建指定类型的渲染器
    renderer_ = CreateRenderer(renderer_type, window_.get());
    if (!renderer_) {
        return false;
    }
    
    owns_renderer_ = true;
    name_ = name;
    
    return true;
}

bool RenderWindow::InitializeWithExistingWindow(IWindow* window, RendererType renderer_type) {
    if (!window) {
        return false;
    }
    
    window_ = nullptr; // 不拥有外部窗口
    external_window_ = window;
    
    // 创建渲染器
    renderer_ = CreateRenderer(renderer_type, window);
    if (!renderer_) {
        return false;
    }
    
    owns_renderer_ = true;
    
    return true;
}

void RenderWindow::Render() {
    IWindow* win = GetWindow();
    if (!win || !renderer_) {
        return;
    }
    
    win->BeginRender();
    renderer_->BeginFrame();
    
    // 在这里可以添加具体的渲染逻辑
    // 例如：清屏、绘制3D模型、UI等
    
    renderer_->EndFrame();
    win->EndRender();
}

void RenderWindow::Update() {
    IWindow* win = GetWindow();
    if (win) {
        win->Update();
    }
}

void RenderWindow::Show() {
    IWindow* win = GetWindow();
    if (win) {
        win->Show();
    }
}

void RenderWindow::Hide() {
    IWindow* win = GetWindow();
    if (win) {
        win->Hide();
    }
}

IWindow* RenderWindow::GetWindow() const {
    return window_ ? window_.get() : external_window_;
}

IRenderer* RenderWindow::GetRenderer() const {
    return renderer_;
}

std::string RenderWindow::GetName() const {
    return name_;
}

void RenderWindow::SetName(const std::string& name) {
    name_ = name;
}

bool RenderWindow::IsValid() const {
    IWindow* win = GetWindow();
    return win && win->IsValid() && renderer_;
}

IRenderer* RenderWindow::CreateRenderer(RendererType type, IWindow* window) {
    if (!window) {
        return nullptr;
    }
    
    void* native_handle = window->GetNativeHandle();
    if (!native_handle) {
        return nullptr;
    }
    
    IRenderer* renderer = nullptr;
    
    switch (type) {
        case RendererType::DirectX:
#ifdef _WIN32
            // 这里应该使用渲染器工厂创建DirectX渲染器
            // renderer = RendererFactory::CreateDirectXRenderer(native_handle);
#endif
            break;
            
        case RendererType::OpenGL:
            // renderer = RendererFactory::CreateOpenGLRenderer(native_handle);
            break;
            
        case RendererType::Vulkan:
            // renderer = RendererFactory::CreateVulkanRenderer(native_handle);
            break;
            
        case RendererType::Software:
            // renderer = RendererFactory::CreateSoftwareRenderer(native_handle);
            break;
            
        default:
            break;
    }
    
    return renderer;
}