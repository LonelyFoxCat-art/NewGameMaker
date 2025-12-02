#pragma once
#include <windows.h>
#include <string>

// 渲染器接口
class IRenderer
{
public:
    virtual ~IRenderer() = default;

    // 初始化渲染器
    virtual bool Initialize(HWND hwnd) = 0;
    
    // 清理资源
    virtual void Cleanup() = 0;
    
    // 开始渲染帧
    virtual void BeginFrame() = 0;
    
    // 结束渲染帧
    virtual void EndFrame() = 0;
    
    // 设置清除颜色
    virtual void SetClearColor(float r, float g, float b, float a = 1.0f) = 0;
    
    // 绘制简单的几何形状
    virtual void DrawQuad(float x, float y, float width, float height) = 0;
    virtual void DrawTriangle(float x1, float y1, float x2, float y2, float x3, float y3) = 0;
    virtual void DrawCircle(float centerX, float centerY, float radius, int segments = 32) = 0;
    
    // 设置变换
    virtual void SetTransform(float x, float y, float rotation, float scale = 1.0f) = 0;
    
    // 加载和使用纹理
    virtual unsigned int LoadTexture(const std::string& filename) = 0;
    virtual void UseTexture(unsigned int textureId) = 0;
    
    // 加载和使用着色器
    virtual unsigned int LoadShader(const std::string& vertexShaderFile, const std::string& fragmentShaderFile) = 0;
    virtual void UseShader(unsigned int shaderId) = 0;
    
    // 设置渲染表面
    virtual void SetSurface(unsigned int width, unsigned int height) = 0;
};