#pragma once
#include "IRenderer.h"
#include <windows.h>

enum class RendererType
{
    OpenGL,
    DirectX,
    Vulkan,
    Software
};

class RendererFactory
{
public:
    // 创建指定类型的渲染器
    static IRenderer* CreateRenderer(RendererType type);
    
    // 销毁渲染器
    static void DestroyRenderer(IRenderer* renderer);
};