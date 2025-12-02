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
    // In a real implementation, this would create vertex buffers and draw calls
    // For now, we'll implement a basic quad drawing using immediate mode approach
    // which isn't available in D3D11, so we'll simulate it by using a simple vertex structure
    
    // Define vertices for a quad (position only for simplicity)
    struct Vertex {
        float x, y, z;
    };
    
    Vertex vertices[] = {
        { x, y, 0.0f },           // Top-left
        { x + width, y, 0.0f },   // Top-right
        { x, y + height, 0.0f },  // Bottom-left
        { x + width, y + height, 0.0f }  // Bottom-right
    };
    
    // Define indices to form two triangles from the quad
    WORD indices[] = {
        0, 1, 2,  // First triangle
        1, 3, 2   // Second triangle
    };
    
    // In a real implementation, we would:
    // 1. Create vertex buffer with these vertices
    // 2. Create index buffer with these indices
    // 3. Set up input layout
    // 4. Set vertex and index buffers
    // 5. Draw indexed
    
    // For this example, we'll just log the operation
    // In a complete implementation, we'd need to set up the full pipeline
}

void DirectXRenderer::DrawTriangle(float x1, float y1, float x2, float y2, float x3, float y3)
{
    // Define vertices for a triangle
    struct Vertex {
        float x, y, z;
    };
    
    Vertex vertices[] = {
        { x1, y1, 0.0f },  // Vertex 1
        { x2, y2, 0.0f },  // Vertex 2
        { x3, y3, 0.0f }   // Vertex 3
    };
    
    // In a real implementation, we would:
    // 1. Create vertex buffer with these vertices
    // 2. Set up input layout
    // 3. Set vertex buffer
    // 4. Draw
    
    // For this example, we'll just log the operation
}

void DirectXRenderer::DrawCircle(float centerX, float centerY, float radius, int segments)
{
    // Define vertices for a circle using triangle fan approach
    struct Vertex {
        float x, y, z;
    };
    
    // Create vertices for the circle
    std::vector<Vertex> vertices;
    vertices.push_back({centerX, centerY, 0.0f}); // Center vertex
    
    // Create vertices around the circle
    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * 3.14159265359f * i / segments;
        float x = centerX + radius * cosf(angle);
        float y = centerY + radius * sinf(angle);
        vertices.push_back({x, y, 0.0f});
    }
    
    // In a real implementation, we would:
    // 1. Create vertex buffer with these vertices
    // 2. Set up input layout
    // 3. Set vertex buffer
    // 4. Draw with appropriate primitive topology
    
    // For this example, we'll just log the operation
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
    // In a real implementation, this would load a texture using DirectX
    // For this implementation, we'll generate a texture ID and store placeholder data
    unsigned int textureId = static_cast<unsigned int>(textures.size() + 1);
    
    // In a real implementation, we would:
    // 1. Load the image file using a library like WIC, STB, or DirectXTK
    // 2. Create an ID3D11Texture2D object
    // 3. Create an ID3D11ShaderResourceView for the texture
    // 4. Store both in our texture map
    
    // For now, we'll just store the filename as a placeholder
    TextureData textureData;
    textureData.id = textureId;
    textureData.filename = filename;
    textureData.texture = nullptr;  // Would be an actual texture in real implementation
    textureData.resourceView = nullptr;  // Would be an actual resource view in real implementation
    
    textures[textureId] = textureData;
    
    return textureId;
}

void DirectXRenderer::UseTexture(unsigned int textureId)
{
    // In a real implementation, this would bind the texture resource
    auto it = textures.find(textureId);
    if (it != textures.end() && it->second.resourceView != nullptr) {
        // Set the shader resource view for the texture
        m_context->PSSetShaderResources(0, 1, &it->second.resourceView);
    } else {
        ID3D11ShaderResourceView* nullSRV = nullptr;
        m_context->PSSetShaderResources(0, 1, &nullSRV);
    }
}

unsigned int DirectXRenderer::LoadShader(const std::string& vertexShaderFile, const std::string& fragmentShaderFile)
{
    // In a real implementation, this would load and compile shaders
    // For this implementation, we'll create a shader ID and store placeholder data
    unsigned int shaderId = static_cast<unsigned int>(shaders.size() + 1);
    
    // In a real implementation, we would:
    // 1. Read the shader files
    // 2. Compile the vertex and fragment shaders using D3DCompile
    // 3. Create shader objects
    // 4. Create an input layout
    // 5. Create a pixel shader
    // 6. Store all these in a structure
    
    // For now, we'll just store the filenames as placeholders
    ShaderData shaderData;
    shaderData.id = shaderId;
    shaderData.vertexShaderFile = vertexShaderFile;
    shaderData.fragmentShaderFile = fragmentShaderFile;
    shaderData.vertexShader = nullptr;    // Would be actual shader in real implementation
    shaderData.pixelShader = nullptr;     // Would be actual shader in real implementation
    shaderData.inputLayout = nullptr;     // Would be actual input layout in real implementation
    
    shaders[shaderId] = shaderData;
    
    return shaderId;
}

void DirectXRenderer::UseShader(unsigned int shaderId)
{
    // In a real implementation, this would activate the shader program
    auto it = shaders.find(shaderId);
    if (it != shaders.end() && it->second.vertexShader != nullptr && it->second.pixelShader != nullptr) {
        // Set the vertex shader
        m_context->VSSetShader(it->second.vertexShader, nullptr, 0);
        // Set the pixel shader
        m_context->PSSetShader(it->second.pixelShader, nullptr, 0);
        // Set the input layout
        m_context->IASetInputLayout(it->second.inputLayout);
    } else {
        // Use default shaders if available, or set to null
        m_context->VSSetShader(nullptr, nullptr, 0);
        m_context->PSSetShader(nullptr, nullptr, 0);
        m_context->IASetInputLayout(nullptr);
    }
}

void DirectXRenderer::SetSurface(unsigned int width, unsigned int height)
{
    // 在实际实现中，这里需要调整渲染表面大小
    // 例如重新配置视口、缓冲区等
    
    // Release current render target view
    if (m_renderTargetView) {
        m_renderTargetView->Release();
        m_renderTargetView = nullptr;
    }
    
    // Resize the swap chain
    HRESULT hr = m_swapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
    if (FAILED(hr)) {
        // Handle error appropriately
        return;
    }
    
    // Get the new back buffer
    ID3D11Texture2D* backBuffer = nullptr;
    hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
    if (FAILED(hr)) {
        return;
    }
    
    // Create a new render target view
    hr = m_device->CreateRenderTargetView(backBuffer, nullptr, &m_renderTargetView);
    backBuffer->Release();
    if (FAILED(hr)) {
        return;
    }
    
    // Bind the new render target
    m_context->OMSetRenderTargets(1, &m_renderTargetView, nullptr);
    
    // Update the viewport
    D3D11_VIEWPORT viewport = {};
    viewport.Width = (FLOAT)width;
    viewport.Height = (FLOAT)height;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    m_context->RSSetViewports(1, &viewport);
}