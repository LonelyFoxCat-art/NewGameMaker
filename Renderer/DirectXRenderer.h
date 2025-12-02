#pragma once
#include "IRenderer.h"
#include <windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <vector>
#include <string>
#include <unordered_map>

// Texture data structure
struct TextureData {
    unsigned int id;
    std::string filename;
    ID3D11Texture2D* texture;
    ID3D11ShaderResourceView* resourceView;
};

// Shader data structure
struct ShaderData {
    unsigned int id;
    std::string vertexShaderFile;
    std::string fragmentShaderFile;
    ID3D11VertexShader* vertexShader;
    ID3D11PixelShader* pixelShader;
    ID3D11InputLayout* inputLayout;
};

// DirectX 11渲染器类
class DirectXRenderer : public IRenderer
{
public:
    DirectXRenderer();
    ~DirectXRenderer();

    // 初始化渲染器
    bool Initialize(HWND hwnd) override;
    
    // 清理资源
    void Cleanup() override;
    
    // 开始渲染帧
    void BeginFrame() override;
    
    // 结束渲染帧
    void EndFrame() override;
    
    // 设置清除颜色
    void SetClearColor(float r, float g, float b, float a = 1.0f) override;
    
    // 绘制简单的几何形状
    void DrawQuad(float x, float y, float width, float height) override;
    void DrawTriangle(float x1, float y1, float x2, float y2, float x3, float y3) override;
    void DrawCircle(float centerX, float centerY, float radius, int segments = 32) override;
    
    // 设置变换
    void SetTransform(float x, float y, float rotation, float scale = 1.0f) override;
    
    // 加载和使用纹理
    unsigned int LoadTexture(const std::string& filename) override;
    void UseTexture(unsigned int textureId) override;
    
    // 加载和使用着色器
    unsigned int LoadShader(const std::string& vertexShaderFile, const std::string& fragmentShaderFile) override;
    void UseShader(unsigned int shaderId) override;
    
    // 设置渲染表面
    void SetSurface(unsigned int width, unsigned int height) override;

private:
    HWND m_hwnd;
    ID3D11Device* m_device;
    ID3D11DeviceContext* m_context;
    IDXGISwapChain* m_swapChain;
    ID3D11RenderTargetView* m_renderTargetView;
    
    float m_clearColor[4];
    
    // 当前变换参数
    float m_transformX, m_transformY;
    float m_rotation;
    float m_scale;
    
    // Texture management
    std::unordered_map<unsigned int, TextureData> textures;
    
    // Shader management
    std::unordered_map<unsigned int, ShaderData> shaders;
};