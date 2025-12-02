#include "DirectXRenderer.h"
#include <d3dcompiler.h>
#include <cmath>

DirectXRenderer::DirectXRenderer() 
    : m_hwnd(nullptr), m_device(nullptr), m_context(nullptr), 
      m_swapChain(nullptr), m_renderTargetView(nullptr)
    , m_transformX(0.0f), m_transformY(0.0f), m_rotation(0.0f), m_scale(1.0f)
{
    m_clearColor[0] = 0.0f;
    m_clearColor[1] = 0.0f;
    m_clearColor[2] = 0.0f;
    m_clearColor[3] = 1.0f;
}

DirectXRenderer::~DirectXRenderer()
{
    Cleanup();
}

bool DirectXRenderer::Initialize(HWND hwnd)
{
    m_hwnd = hwnd;
    
    // 获取窗口大小
    RECT rect;
    GetClientRect(hwnd, &rect);
    UINT width = rect.right - rect.left;
    UINT height = rect.bottom - rect.top;
    
    // 创建Direct3D设备和上下文
    DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
    swapChainDesc.BufferCount = 1;
    swapChainDesc.BufferDesc.Width = width;
    swapChainDesc.BufferDesc.Height = height;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.OutputWindow = hwnd;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.Windowed = TRUE;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    
    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr,                    // 适配器
        D3D_DRIVER_TYPE_HARDWARE,   // 驱动类型
        nullptr,                    // 软件驱动路径
        0,                          // 标志
        &featureLevel,              // 功能级别
        1,                          // 功能级别数
        D3D11_SDK_VERSION,          // SDK版本
        &swapChainDesc,             // 交换链描述
        &m_swapChain,               // 交换链
        &m_device,                  // 设备
        nullptr,                    // 功能级别
        &m_context                  // 设备上下文
    );
    
    if (FAILED(hr))
    {
        return false;
    }
    
    // 创建渲染目标视图
    ID3D11Texture2D* backBuffer = nullptr;
    hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
    if (FAILED(hr))
    {
        return false;
    }
    
    hr = m_device->CreateRenderTargetView(backBuffer, nullptr, &m_renderTargetView);
    backBuffer->Release();
    
    if (FAILED(hr))
    {
        return false;
    }
    
    // 绑定渲染目标
    m_context->OMSetRenderTargets(1, &m_renderTargetView, nullptr);
    
    // 设置视口
    D3D11_VIEWPORT viewport = {};
    viewport.Width = (FLOAT)width;
    viewport.Height = (FLOAT)height;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    m_context->RSSetViewports(1, &viewport);
    
    return true;
}

void DirectXRenderer::Cleanup()
{
    if (m_renderTargetView) m_renderTargetView->Release();
    if (m_swapChain) m_swapChain->Release();
    if (m_context) m_context->Release();
    if (m_device) m_device->Release();
    
    m_renderTargetView = nullptr;
    m_swapChain = nullptr;
    m_context = nullptr;
    m_device = nullptr;
}

void DirectXRenderer::BeginFrame()
{
    m_context->ClearRenderTargetView(m_renderTargetView, m_clearColor);
}

void DirectXRenderer::EndFrame()
{
    m_swapChain->Present(0, 0);
}

void DirectXRenderer::SetClearColor(float r, float g, float b, float a)
{
    m_clearColor[0] = r;
    m_clearColor[1] = g;
    m_clearColor[2] = b;
    m_clearColor[3] = a;
}

void DirectXRenderer::DrawQuad(float x, float y, float width, float height)
{
    // 在实际实现中，这里需要创建顶点缓冲区和绘制调用
    // 由于DirectX需要更多的设置，这里仅作为框架示例
}

void DirectXRenderer::DrawTriangle(float x1, float y1, float x2, float y2, float x3, float y3)
{
    // 在实际实现中，这里需要创建顶点缓冲区和绘制调用
    // 由于DirectX需要更多的设置，这里仅作为框架示例
}

void DirectXRenderer::DrawCircle(float centerX, float centerY, float radius, int segments)
{
    // 在实际实现中，这里需要创建顶点缓冲区和绘制调用
    // 由于DirectX需要更多的设置，这里仅作为框架示例
}

void DirectXRenderer::SetTransform(float x, float y, float rotation, float scale)
{
    m_transformX = x;
    m_transformY = y;
    m_rotation = rotation;
    m_scale = scale;
}

unsigned int DirectXRenderer::LoadTexture(const std::string& filename)
{
    // 这里应该实现纹理加载逻辑
    // 为了简化，我们返回0表示未实现
    return 0;
}

void DirectXRenderer::UseTexture(unsigned int textureId)
{
    // 在实际实现中，这里需要绑定纹理资源
}