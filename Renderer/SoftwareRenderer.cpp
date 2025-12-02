#include "SoftwareRenderer.h"
#include <cmath>
#include <algorithm>
#include <stdexcept>
#include <fstream>
#include <iostream>

SoftwareRenderer::SoftwareRenderer()
    : windowHandle(nullptr), deviceContext(nullptr), memoryDC(nullptr), 
      bitmap(nullptr), oldBitmap(nullptr), pixelBuffer(nullptr), 
      width(0), height(0), nextTextureId(1), nextShaderId(1), currentTextureId(0), currentShaderId(0)
{
    clearColor[0] = 0.0f; clearColor[1] = 0.0f; clearColor[2] = 0.0f; clearColor[3] = 1.0f;
    transform[0] = 0.0f; transform[1] = 0.0f; transform[2] = 0.0f; transform[3] = 1.0f;
}

SoftwareRenderer::~SoftwareRenderer()
{
    Cleanup();
}

bool SoftwareRenderer::Initialize(HWND hwnd)
{
    windowHandle = hwnd;
    
    // Get window dimensions
    RECT rect;
    GetClientRect(windowHandle, &rect);
    width = rect.right - rect.left;
    height = rect.bottom - rect.top;
    
    // Get device context
    deviceContext = GetDC(windowHandle);
    if (!deviceContext) {
        return false;
    }
    
    // Create memory DC for off-screen rendering
    memoryDC = CreateCompatibleDC(deviceContext);
    if (!memoryDC) {
        ReleaseDC(windowHandle, deviceContext);
        return false;
    }
    
    // Create compatible bitmap
    BITMAPINFO bmi = {0};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height; // Negative for top-down bitmap
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    
    void* bits = nullptr;
    bitmap = CreateDIBSection(deviceContext, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!bitmap) {
        DeleteDC(memoryDC);
        ReleaseDC(windowHandle, deviceContext);
        return false;
    }
    
    pixelBuffer = static_cast<BYTE*>(bits);
    
    // Select bitmap into memory DC
    oldBitmap = static_cast<HBITMAP>(SelectObject(memoryDC, bitmap));
    
    // Clear the buffer
    ClearBuffer();
    
    return true;
}

void SoftwareRenderer::Cleanup()
{
    if (oldBitmap) {
        SelectObject(memoryDC, oldBitmap);
    }
    
    if (bitmap) {
        DeleteObject(bitmap);
    }
    
    if (memoryDC) {
        DeleteDC(memoryDC);
    }
    
    if (deviceContext) {
        ReleaseDC(windowHandle, deviceContext);
    }
    
    // Clean up textures
    for (auto& pair : textures) {
        DeleteObject(pair.second);
    }
    textures.clear();
    
    // Clean up shaders
    shaders.clear();
    
    pixelBuffer = nullptr;
    width = 0;
    height = 0;
}

void SoftwareRenderer::BeginFrame()
{
    ClearBuffer();
}

void SoftwareRenderer::EndFrame()
{
    UpdateWindow();
}

void SoftwareRenderer::SetClearColor(float r, float g, float b, float a)
{
    clearColor[0] = r;
    clearColor[1] = g;
    clearColor[2] = b;
    clearColor[3] = a;
}

void SoftwareRenderer::DrawQuad(float x, float y, float width, float height)
{
    int x1 = TransformX(x);
    int y1 = TransformY(y);
    int x2 = TransformX(x + width);
    int y2 = TransformY(y + height);
    
    // Clamp to screen bounds
    x1 = std::max(0, std::min(x1, this->width));
    y1 = std::max(0, std::min(y1, this->height));
    x2 = std::max(0, std::min(x2, this->width));
    y2 = std::max(0, std::min(y2, this->height));
    
    BYTE r = static_cast<BYTE>(std::min(255.0f, std::max(0.0f, transform[3] * 255.0f)));
    BYTE g = static_cast<BYTE>(std::min(255.0f, std::max(0.0f, transform[3] * 128.0f)));
    BYTE b = static_cast<BYTE>(std::min(255.0f, std::max(0.0f, transform[3] * 64.0f)));
    
    // Fill the quad
    for (int py = y1; py < y2; py++) {
        for (int px = x1; px < x2; px++) {
            int idx = (py * this->width + px) * 4;
            if (idx >= 0 && idx + 3 < this->width * this->height * 4) {
                pixelBuffer[idx + 2] = r; // R
                pixelBuffer[idx + 1] = g; // G
                pixelBuffer[idx + 0] = b; // B
            }
        }
    }
}

void SoftwareRenderer::DrawTriangle(float x1, float y1, float x2, float y2, float x3, float y3)
{
    int tx1 = TransformX(x1);
    int ty1 = TransformY(y1);
    int tx2 = TransformX(x2);
    int ty2 = TransformY(y2);
    int tx3 = TransformX(x3);
    int ty3 = TransformY(y3);
    
    BYTE r = static_cast<BYTE>(std::min(255.0f, std::max(0.0f, transform[3] * 255.0f)));
    BYTE g = static_cast<BYTE>(std::min(255.0f, std::max(0.0f, transform[3] * 64.0f)));
    BYTE b = static_cast<BYTE>(std::min(255.0f, std::max(0.0f, transform[3] * 128.0f)));
    
    DrawFilledTriangle(tx1, ty1, tx2, ty2, tx3, ty3, r, g, b);
}

void SoftwareRenderer::DrawCircle(float centerX, float centerY, float radius, int segments)
{
    int cx = TransformX(centerX);
    int cy = TransformY(centerY);
    int r = static_cast<int>(radius * transform[3]);
    
    BYTE red = static_cast<BYTE>(std::min(255.0f, std::max(0.0f, transform[3] * 128.0f)));
    BYTE green = static_cast<BYTE>(std::min(255.0f, std::max(0.0f, transform[3] * 255.0f)));
    BYTE blue = static_cast<BYTE>(std::min(255.0f, std::max(0.0f, transform[3] * 64.0f)));
    
    DrawFilledCircle(cx, cy, r, red, green, blue);
}

void SoftwareRenderer::SetTransform(float x, float y, float rotation, float scale)
{
    transform[0] = x;
    transform[1] = y;
    transform[2] = rotation;
    transform[3] = scale;
}

unsigned int SoftwareRenderer::LoadTexture(const std::string& filename)
{
    // In a real implementation, this would load an image file
    // For this example, we'll create a placeholder texture based on the filename
    unsigned int textureId = nextTextureId++;
    
    // Check if the file exists and load accordingly
    // For now, we'll create different patterns based on the filename
    int width = 64;
    int height = 64;
    
    // Create a simple placeholder bitmap (checkerboard pattern)
    BITMAPINFO bmi = {0};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height; // Negative for top-down bitmap
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    
    HDC tempDC = CreateCompatibleDC(nullptr);
    HBITMAP tempBitmap = CreateDIBSection(nullptr, &bmi, DIB_RGB_COLORS, nullptr, nullptr, 0);
    
    if (tempBitmap) {
        HBITMAP oldTempBitmap = static_cast<HBITMAP>(SelectObject(tempDC, tempBitmap));
        
        // Draw a simple pattern based on the filename
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                BYTE r, g, b;
                
                // Create different patterns based on filename
                if (filename.find("checker") != std::string::npos) {
                    r = ((x / 8) + (y / 8)) % 2 ? 255 : 128;
                    g = ((x / 8) + (y / 8)) % 2 ? 128 : 64;
                    b = ((x / 8) + (y / 8)) % 2 ? 64 : 255;
                } else if (filename.find("gradient") != std::string::npos) {
                    r = static_cast<BYTE>((x * 255) / width);
                    g = static_cast<BYTE>((y * 255) / height);
                    b = static_cast<BYTE>(128);
                } else {
                    // Default checkerboard pattern
                    r = ((x / 4) + (y / 4)) % 2 ? 255 : 0;
                    g = ((x / 4) + (y / 4)) % 2 ? 0 : 255;
                    b = 255 - r;
                }
                
                COLORREF color = RGB(r, g, b);
                SetPixel(tempDC, x, y, color);
            }
        }
        
        SelectObject(tempDC, oldTempBitmap);
        DeleteDC(tempDC);
        
        textures[textureId] = tempBitmap;
    }
    
    return textureId;
}

void SoftwareRenderer::UseTexture(unsigned int textureId)
{
    // In a real implementation, this would bind the texture for rendering
    // For this example, we check if the texture exists and store it for use in rendering
    if (textureId == 0 || textures.find(textureId) != textures.end()) {
        currentTextureId = textureId;
    } else {
        currentTextureId = 0; // No texture
    }
}

unsigned int SoftwareRenderer::LoadShader(const std::string& vertexShaderFile, const std::string& fragmentShaderFile)
{
    // Read shader files
    std::string vertexCode = ReadShaderFile(vertexShaderFile);
    std::string fragmentCode = ReadShaderFile(fragmentShaderFile);
    
    if (vertexCode.empty() || fragmentCode.empty()) {
        // If file reading fails, use default shaders
        vertexCode = R"(
            // Default vertex shader for software renderer
            // In a real implementation, this would be parsed and executed in software
            // This is just a placeholder
        )";
        
        fragmentCode = R"(
            // Default fragment shader for software renderer
            // In a real implementation, this would be parsed and executed in software
            // This is just a placeholder
        )";
    }
    
    // In a software renderer, shader loading would be a complex process
    // involving parsing and implementing shader programs in software
    // For this implementation, we'll create a basic shader ID system
    unsigned int shaderId = nextShaderId++; // Use nextShaderId as shader ID counter
    
    // In a real implementation, we would:
    // 1. Parse the shader code
    // 2. Convert to software-renderer compatible format
    // 3. Store the compiled shader for later use
    
    // For now, we'll store the shader source code as a placeholder
    ShaderData shaderData;
    shaderData.vertexShaderFile = vertexShaderFile;
    shaderData.fragmentShaderFile = fragmentShaderFile;
    shaderData.id = shaderId;
    
    shaders[shaderId] = shaderData;
    
    std::cout << "Loaded shader from " << vertexShaderFile << " and " << fragmentShaderFile << " with ID: " << shaderId << std::endl;
    
    return shaderId;
}

void SoftwareRenderer::UseShader(unsigned int shaderId)
{
    // In a real implementation, this would activate the shader program
    // For this example, we check if the shader exists and store it for use
    if (shaderId == 0 || shaders.find(shaderId) != shaders.end()) {
        currentShaderId = shaderId;
    } else {
        currentShaderId = 0; // No shader
    }
}

void SoftwareRenderer::SetSurface(unsigned int width, unsigned int height)
{
    // Clean up existing resources
    if (oldBitmap) {
        SelectObject(memoryDC, oldBitmap);
    }
    
    if (bitmap) {
        DeleteObject(bitmap);
    }
    
    if (memoryDC) {
        DeleteDC(memoryDC);
    }
    
    // Update dimensions
    this->width = width;
    this->height = height;
    
    // Recreate the bitmap with new dimensions
    BITMAPINFO bmi = {0};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height; // Negative for top-down bitmap
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    
    void* bits = nullptr;
    bitmap = CreateDIBSection(deviceContext, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!bitmap) {
        return; // Failed to create new bitmap
    }
    
    pixelBuffer = static_cast<BYTE*>(bits);
    
    // Create new memory DC and select bitmap
    memoryDC = CreateCompatibleDC(deviceContext);
    if (!memoryDC) {
        DeleteObject(bitmap);
        return; // Failed to create new DC
    }
    
    oldBitmap = static_cast<HBITMAP>(SelectObject(memoryDC, bitmap));
    
    // Clear the new buffer
    ClearBuffer();
}

// Helper methods implementation
void SoftwareRenderer::ClearBuffer()
{
    if (pixelBuffer) {
        BYTE r = static_cast<BYTE>(clearColor[0] * 255);
        BYTE g = static_cast<BYTE>(clearColor[1] * 255);
        BYTE b = static_cast<BYTE>(clearColor[2] * 255);
        
        for (int i = 0; i < width * height; i++) {
            int idx = i * 4;
            pixelBuffer[idx + 0] = b; // B
            pixelBuffer[idx + 1] = g; // G
            pixelBuffer[idx + 2] = r; // R
            pixelBuffer[idx + 3] = 255; // A
        }
    }
}

void SoftwareRenderer::PutPixel(int x, int y, BYTE r, BYTE g, BYTE b)
{
    if (x >= 0 && x < width && y >= 0 && y < height) {
        int idx = (y * width + x) * 4;
        pixelBuffer[idx + 0] = b; // B
        pixelBuffer[idx + 1] = g; // G
        pixelBuffer[idx + 2] = r; // R
        // Alpha channel: pixelBuffer[idx + 3] = 255;
    }
}

void SoftwareRenderer::DrawLine(int x0, int y0, int x1, int y1, BYTE r, BYTE g, BYTE b)
{
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;
    
    while (true) {
        PutPixel(x0, y0, r, g, b);
        
        if (x0 == x1 && y0 == y1) break;
        
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

void SoftwareRenderer::DrawFilledTriangle(int x0, int y0, int x1, int y1, int x2, int y2, BYTE r, BYTE g, BYTE b)
{
    // Sort vertices by y coordinate
    if (y1 > y2) { std::swap(x1, x2); std::swap(y1, y2); }
    if (y0 > y1) { std::swap(x0, x1); std::swap(y0, y1); }
    if (y1 > y2) { std::swap(x1, x2); std::swap(y1, y2); }
    
    // Function to draw a horizontal line
    auto drawHorizontalLine = [this, r, g, b](int y, int x1, int x2) {
        for (int x = x1; x <= x2; x++) {
            PutPixel(x, y, r, g, b);
        }
    };
    
    // Draw upper part of triangle
    if (y1 != y0) {
        for (int y = y0; y <= y1; y++) {
            float t = (y - y0) / float(y1 - y0);
            int x_start = x0 + (x1 - x0) * t;
            int x_end = x0 + (x2 - x0) * t;
            
            if (x_start > x_end) std::swap(x_start, x_end);
            drawHorizontalLine(y, x_start, x_end);
        }
    }
    
    // Draw lower part of triangle
    if (y2 != y1) {
        for (int y = y1; y <= y2; y++) {
            float t = (y - y1) / float(y2 - y1);
            int x_start = x1 + (x2 - x1) * t;
            int x_end = x0 + (x2 - x0) * t;
            
            if (x_start > x_end) std::swap(x_start, x_end);
            drawHorizontalLine(y, x_start, x_end);
        }
    }
}

void SoftwareRenderer::DrawFilledCircle(int centerX, int centerY, int radius, BYTE r, BYTE g, BYTE b)
{
    for (int y = -radius; y <= radius; y++) {
        for (int x = -radius; x <= radius; x++) {
            if (x * x + y * y <= radius * radius) {
                PutPixel(centerX + x, centerY + y, r, g, b);
            }
        }
    }
}

void SoftwareRenderer::UpdateWindow()
{
    if (deviceContext && memoryDC) {
        BitBlt(deviceContext, 0, 0, width, height, memoryDC, 0, 0, SRCCOPY);
    }
}

int SoftwareRenderer::TransformX(float x)
{
    // Apply translation and scaling
    return static_cast<int>((x + transform[0]) * transform[3]);
}

int SoftwareRenderer::TransformY(float y)
{
    // Apply translation and scaling
    // Note: Y axis is inverted in Windows coordinates
    return static_cast<int>((y + transform[1]) * transform[3]);
}

std::string SoftwareRenderer::ReadShaderFile(const std::string& filePath)
{
    std::string content;
    std::ifstream fileStream(filePath, std::ios::in);
    
    if (!fileStream.is_open()) {
        // Return empty string if file cannot be opened
        return content;
    }
    
    std::string line = "";
    while (std::getline(fileStream, line)) {
        content.append(line + "\n");
    }
    
    fileStream.close();
    return content;
}