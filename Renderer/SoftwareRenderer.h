#pragma once
#include "IRenderer.h"
#include <windows.h>
#include <vector>
#include <unordered_map>
#include <string>

class SoftwareRenderer : public IRenderer
{
public:
    SoftwareRenderer();
    virtual ~SoftwareRenderer();

    // IRenderer interface implementation
    virtual bool Initialize(HWND hwnd) override;
    virtual void Cleanup() override;
    virtual void BeginFrame() override;
    virtual void EndFrame() override;
    virtual void SetClearColor(float r, float g, float b, float a = 1.0f) override;
    virtual void DrawQuad(float x, float y, float width, float height) override;
    virtual void DrawTriangle(float x1, float y1, float x2, float y2, float x3, float y3) override;
    virtual void DrawCircle(float centerX, float centerY, float radius, int segments = 32) override;
    virtual void SetTransform(float x, float y, float rotation, float scale = 1.0f) override;
    virtual unsigned int LoadTexture(const std::string& filename) override;
    virtual void UseTexture(unsigned int textureId) override;
    virtual unsigned int LoadShader(const std::string& vertexShaderFile, const std::string& fragmentShaderFile) override;
    virtual void UseShader(unsigned int shaderId) override;
    virtual void SetSurface(unsigned int width, unsigned int height) override;

private:
    HWND windowHandle;
    HDC deviceContext;
    HDC memoryDC;
    HBITMAP bitmap;
    HBITMAP oldBitmap;
    BYTE* pixelBuffer;
    int width;
    int height;

    // Rendering state
    float clearColor[4];
    float transform[4]; // x, y, rotation, scale

    // Texture management
    std::unordered_map<unsigned int, HBITMAP> textures;
    unsigned int nextTextureId;

    // Helper methods
    void ClearBuffer();
    void PutPixel(int x, int y, BYTE r, BYTE g, BYTE b);
    void DrawLine(int x0, int y0, int x1, int y1, BYTE r, BYTE g, BYTE b);
    void DrawFilledTriangle(int x0, int y0, int x1, int y1, int x2, int y2, BYTE r, BYTE g, BYTE b);
    void DrawFilledCircle(int centerX, int centerY, int radius, BYTE r, BYTE g, BYTE b);
    void UpdateWindow();
    int TransformX(float x);
    int TransformY(float y);
};