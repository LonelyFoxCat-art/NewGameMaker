# NewGameMaker 渲染器系统

## 概述

NewGameMaker渲染器系统提供了一个灵活的、可扩展的渲染架构，支持多种渲染后端（如OpenGL和DirectX）。

## 架构

### IRenderer 接口
定义了渲染器的基本接口，所有渲染器实现都必须继承此接口。

### OpenGL渲染器
基于OpenGL的渲染器实现，提供跨平台兼容性。

### DirectX渲染器
基于DirectX的渲染器实现，提供Windows平台上的高性能渲染。

### 渲染器工厂
用于创建和管理不同类型的渲染器实例。

## 使用方法

### 初始化渲染器
```cpp
IRenderer* renderer = RendererFactory::CreateRenderer(RendererType::OpenGL);
if (renderer && renderer->Initialize(hWnd))
{
    // 渲染器初始化成功
}
```

### 渲染流程
```cpp
// 开始帧
renderer->BeginFrame();

// 绘制内容
renderer->SetClearColor(0.2f, 0.3f, 0.4f, 1.0f);
renderer->DrawQuad(100.0f, 100.0f, 200.0f, 150.0f);

// 结束帧
renderer->EndFrame();
```

### 清理
```cpp
renderer->Cleanup();
RendererFactory::DestroyRenderer(renderer);
```

## 扩展

要添加新的渲染器实现，需要：

1. 继承IRenderer接口
2. 实现所有纯虚函数
3. 在RendererFactory中添加创建逻辑

## 优势

- **可扩展性**: 支持多种渲染后端
- **灵活性**: 可以在运行时选择渲染器类型
- **封装性**: 将底层渲染API细节封装在实现中
- **易于维护**: 清晰的接口分离