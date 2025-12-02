#pragma once
#include "IRenderer.h"
#include <windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <vector>
#include <string>

// OpenGL渲染器类
class OpenGLRenderer : public IRenderer
{
public:
    OpenGLRenderer();
    ~OpenGLRenderer();

    // 初始化OpenGL上下文
    bool Initialize(HWND hwnd);
    
    // 清理资源
    void Cleanup();
    
    // 开始渲染帧
    void BeginFrame();
    
    // 结束渲染帧
    void EndFrame();
    
    // 设置清除颜色
    void SetClearColor(float r, float g, float b, float a = 1.0f);
    
    // 绘制简单的几何形状
    void DrawQuad(float x, float y, float width, float height);
    void DrawTriangle(float x1, float y1, float x2, float y2, float x3, float y3);
    void DrawCircle(float centerX, float centerY, float radius, int segments = 32);
    
    // 设置变换矩阵
    void SetTransform(float x, float y, float rotation, float scale = 1.0f);
    
    // 加载和使用纹理
    unsigned int LoadTexture(const std::string& filename);
    void UseTexture(unsigned int textureId);
    
    // 加载和使用着色器
    unsigned int LoadShader(const std::string& vertexShaderFile, const std::string& fragmentShaderFile);
    void UseShader(unsigned int shaderId);
    
    // 设置渲染表面
    void SetSurface(unsigned int width, unsigned int height);

private:
    HWND m_hwnd;
    HDC m_hdc;
    HGLRC m_hglrc;
    float m_clearColor[4];
    
    // 当前变换参数
    float m_transformX, m_transformY;
    float m_rotation;
    float m_scale;
};