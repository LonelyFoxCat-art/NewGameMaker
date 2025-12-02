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
    
    // Create vertex buffer
    D3D11_BUFFER_DESC vertexBufferDesc = {};
    vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    vertexBufferDesc.ByteWidth = sizeof(Vertex) * 4;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    
    D3D11_SUBRESOURCE_DATA vertexData = {};
    vertexData.pSysMem = vertices;
    
    ID3D11Buffer* vertexBuffer = nullptr;
    HRESULT hr = m_device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);
    if (FAILED(hr)) {
        return;
    }
    
    // Create index buffer
    D3D11_BUFFER_DESC indexBufferDesc = {};
    indexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    indexBufferDesc.ByteWidth = sizeof(WORD) * 6;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    
    D3D11_SUBRESOURCE_DATA indexData = {};
    indexData.pSysMem = indices;
    
    ID3D11Buffer* indexBuffer = nullptr;
    hr = m_device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);
    if (FAILED(hr)) {
        vertexBuffer->Release();
        return;
    }
    
    // Set vertex and index buffers
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    m_context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
    m_context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R16_UINT, 0);
    
    // Set primitive topology
    m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    
    // Draw indexed
    m_context->DrawIndexed(6, 0, 0);
    
    // Clean up temporary buffers
    vertexBuffer->Release();
    indexBuffer->Release();
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
    
    // Create vertex buffer
    D3D11_BUFFER_DESC vertexBufferDesc = {};
    vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    vertexBufferDesc.ByteWidth = sizeof(Vertex) * 3;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    
    D3D11_SUBRESOURCE_DATA vertexData = {};
    vertexData.pSysMem = vertices;
    
    ID3D11Buffer* vertexBuffer = nullptr;
    HRESULT hr = m_device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);
    if (FAILED(hr)) {
        return;
    }
    
    // Set vertex buffer
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    m_context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
    
    // Set primitive topology
    m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    
    // Draw
    m_context->Draw(3, 0);
    
    // Clean up temporary buffer
    vertexBuffer->Release();
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
    
    // Create vertex buffer
    D3D11_BUFFER_DESC vertexBufferDesc = {};
    vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    vertexBufferDesc.ByteWidth = sizeof(Vertex) * static_cast<UINT>(vertices.size());
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    
    D3D11_SUBRESOURCE_DATA vertexData = {};
    vertexData.pSysMem = vertices.data();
    
    ID3D11Buffer* vertexBuffer = nullptr;
    HRESULT hr = m_device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);
    if (FAILED(hr)) {
        return;
    }
    
    // Set vertex buffer
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    m_context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
    
    // Set primitive topology to triangle fan
    m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    
    // Draw with triangle fan (we need to draw vertices.size()-1 points)
    m_context->Draw(static_cast<UINT>(vertices.size()) - 1, 1); // Start from index 1 to skip center
    
    // Clean up temporary buffer
    vertexBuffer->Release();
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
    
    // Create a simple colored texture for demonstration purposes
    // In a real implementation, we would load from file using WIC or DirectXTK
    const int width = 256;
    const int height = 256;
    const int pixelSize = 4; // RGBA
    std::vector<unsigned char> textureData(width * height * pixelSize);
    
    // Create a simple pattern for the texture
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int index = (y * width + x) * pixelSize;
            // Create a simple gradient pattern
            textureData[index] = static_cast<unsigned char>((x * 255) / width);     // R
            textureData[index + 1] = static_cast<unsigned char>((y * 255) / height); // G
            textureData[index + 2] = 128;                                           // B
            textureData[index + 3] = 255;                                           // A
        }
    }
    
    // Create texture description
    D3D11_TEXTURE2D_DESC textureDesc = {};
    textureDesc.Width = width;
    textureDesc.Height = height;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;
    
    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = textureData.data();
    initData.SysMemPitch = width * pixelSize;
    initData.SysMemSlicePitch = 0;
    
    ID3D11Texture2D* texture = nullptr;
    HRESULT hr = m_device->CreateTexture2D(&textureDesc, &initData, &texture);
    if (FAILED(hr)) {
        return 0; // Return 0 to indicate failure
    }
    
    // Create shader resource view
    ID3D11ShaderResourceView* resourceView = nullptr;
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = textureDesc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;
    srvDesc.Texture2D.MostDetailedMip = 0;
    
    hr = m_device->CreateShaderResourceView(texture, &srvDesc, &resourceView);
    if (FAILED(hr)) {
        texture->Release();
        return 0; // Return 0 to indicate failure
    }
    
    // Store texture data
    TextureData textureDataStruct;
    textureDataStruct.id = textureId;
    textureDataStruct.filename = filename;
    textureDataStruct.texture = texture;
    textureDataStruct.resourceView = resourceView;
    
    textures[textureId] = textureDataStruct;
    
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
    
    // Create default vertex shader bytecode (a simple pass-through shader)
    // This is a basic vertex shader that passes position through
    const char* defaultVS = 
        "struct VS_INPUT { float3 pos : POSITION; };"
        "struct VS_OUTPUT { float4 pos : SV_POSITION; };"
        "VS_OUTPUT main(VS_INPUT input) {"
        "    VS_OUTPUT output;"
        "    output.pos = float4(input.pos, 1.0f);"
        "    return output;"
        "}";
    
    // Create default pixel shader bytecode (a simple solid color shader)
    const char* defaultPS = 
        "struct VS_OUTPUT { float4 pos : SV_POSITION; };"
        "float4 main(VS_OUTPUT input) : SV_TARGET {"
        "    return float4(1.0f, 1.0f, 1.0f, 1.0f);"
        "}";
    
    ID3DBlob* compiledVS = nullptr;
    ID3DBlob* compiledPS = nullptr;
    ID3DBlob* errorBlob = nullptr;
    
    // Compile vertex shader
    HRESULT hr = D3DCompile(defaultVS, strlen(defaultVS), nullptr, nullptr, nullptr, "main", "vs_4_0", 
                           0, 0, &compiledVS, &errorBlob);
    if (FAILED(hr)) {
        if (errorBlob) {
            OutputDebugStringA((char*)errorBlob->GetBufferPointer());
            errorBlob->Release();
        }
        return 0; // Return 0 to indicate failure
    }
    
    // Compile pixel shader
    hr = D3DCompile(defaultPS, strlen(defaultPS), nullptr, nullptr, nullptr, "main", "ps_4_0", 
                   0, 0, &compiledPS, &errorBlob);
    if (FAILED(hr)) {
        if (errorBlob) {
            OutputDebugStringA((char*)errorBlob->GetBufferPointer());
            errorBlob->Release();
        }
        if (compiledVS) compiledVS->Release();
        return 0; // Return 0 to indicate failure
    }
    
    // Create shader objects
    ID3D11VertexShader* vertexShader = nullptr;
    hr = m_device->CreateVertexShader(compiledVS->GetBufferPointer(), compiledVS->GetBufferSize(), 
                                      nullptr, &vertexShader);
    if (FAILED(hr)) {
        if (compiledVS) compiledVS->Release();
        if (compiledPS) compiledPS->Release();
        return 0; // Return 0 to indicate failure
    }
    
    ID3D11PixelShader* pixelShader = nullptr;
    hr = m_device->CreatePixelShader(compiledPS->GetBufferPointer(), compiledPS->GetBufferSize(), 
                                     nullptr, &pixelShader);
    if (FAILED(hr)) {
        if (compiledVS) compiledVS->Release();
        if (compiledPS) compiledPS->Release();
        if (vertexShader) vertexShader->Release();
        return 0; // Return 0 to indicate failure
    }
    
    // Define input layout
    D3D11_INPUT_ELEMENT_DESC inputLayoutDesc[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    
    ID3D11InputLayout* inputLayout = nullptr;
    hr = m_device->CreateInputLayout(inputLayoutDesc, 1, 
                                     compiledVS->GetBufferPointer(), compiledVS->GetBufferSize(), 
                                     &inputLayout);
    if (FAILED(hr)) {
        if (compiledVS) compiledVS->Release();
        if (compiledPS) compiledPS->Release();
        if (vertexShader) vertexShader->Release();
        if (pixelShader) pixelShader->Release();
        return 0; // Return 0 to indicate failure
    }
    
    // Store shader data
    ShaderData shaderData;
    shaderData.id = shaderId;
    shaderData.vertexShaderFile = vertexShaderFile;
    shaderData.fragmentShaderFile = fragmentShaderFile;
    shaderData.vertexShader = vertexShader;
    shaderData.pixelShader = pixelShader;
    shaderData.inputLayout = inputLayout;
    
    shaders[shaderId] = shaderData;
    
    // Clean up compiled shaders
    compiledVS->Release();
    compiledPS->Release();
    
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