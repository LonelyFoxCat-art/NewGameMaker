#include "OpenGLRenderer.h"
#include <cmath>

OpenGLRenderer::OpenGLRenderer() 
    : m_hwnd(nullptr), m_hdc(nullptr), m_hglrc(nullptr)
    , m_transformX(0.0f), m_transformY(0.0f), m_rotation(0.0f), m_scale(1.0f)
{
    m_clearColor[0] = 0.0f;
    m_clearColor[1] = 0.0f;
    m_clearColor[2] = 0.0f;
    m_clearColor[3] = 1.0f;
}

OpenGLRenderer::~OpenGLRenderer()
{
    Cleanup();
}

bool OpenGLRenderer::Initialize(HWND hwnd)
{
    m_hwnd = hwnd;
    
    // 获取设备上下文
    m_hdc = GetDC(hwnd);
    if (!m_hdc)
    {
        return false;
    }
    
    // 设置像素格式
    PIXELFORMATDESCRIPTOR pfd;
    ZeroMemory(&pfd, sizeof(pfd));
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 32;
    pfd.iLayerType = PFD_MAIN_PLANE;
    
    int pixelFormat = ChoosePixelFormat(m_hdc, &pfd);
    if (!pixelFormat)
    {
        ReleaseDC(m_hwnd, m_hdc);
        return false;
    }
    
    if (!SetPixelFormat(m_hdc, pixelFormat, &pfd))
    {
        ReleaseDC(m_hwnd, m_hdc);
        return false;
    }
    
    // 创建OpenGL渲染上下文
    m_hglrc = wglCreateContext(m_hdc);
    if (!m_hglrc)
    {
        ReleaseDC(m_hwnd, m_hdc);
        return false;
    }
    
    // 激活渲染上下文
    if (!wglMakeCurrent(m_hdc, m_hglrc))
    {
        wglDeleteContext(m_hglrc);
        ReleaseDC(m_hwnd, m_hdc);
        return false;
    }
    
    // 初始化OpenGL状态
    glClearColor(m_clearColor[0], m_clearColor[1], m_clearColor[2], m_clearColor[3]);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    // 设置正交投影
    RECT rect;
    GetClientRect(m_hwnd, &rect);
    gluOrtho2D(0.0, (GLdouble)(rect.right - rect.left), (GLdouble)(rect.bottom - rect.top), 0.0);
    
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    return true;
}

void OpenGLRenderer::Cleanup()
{
    if (m_hglrc)
    {
        wglMakeCurrent(nullptr, nullptr);
        wglDeleteContext(m_hglrc);
        m_hglrc = nullptr;
    }
    
    if (m_hdc)
    {
        ReleaseDC(m_hwnd, m_hdc);
        m_hdc = nullptr;
    }
}

void OpenGLRenderer::BeginFrame()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
}

void OpenGLRenderer::EndFrame()
{
    SwapBuffers(m_hdc);
}

void OpenGLRenderer::SetClearColor(float r, float g, float b, float a)
{
    m_clearColor[0] = r;
    m_clearColor[1] = g;
    m_clearColor[2] = b;
    m_clearColor[3] = a;
    glClearColor(r, g, b, a);
}

void OpenGLRenderer::DrawQuad(float x, float y, float width, float height)
{
    glPushMatrix();
    glTranslatef(m_transformX, m_transformY, 0.0f);
    glRotatef(m_rotation, 0.0f, 0.0f, 1.0f);
    glScalef(m_scale, m_scale, 1.0f);
    
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x, y + height);
    glEnd();
    
    glPopMatrix();
}

void OpenGLRenderer::DrawTriangle(float x1, float y1, float x2, float y2, float x3, float y3)
{
    glPushMatrix();
    glTranslatef(m_transformX, m_transformY, 0.0f);
    glRotatef(m_rotation, 0.0f, 0.0f, 1.0f);
    glScalef(m_scale, m_scale, 1.0f);
    
    glBegin(GL_TRIANGLES);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glVertex2f(x3, y3);
    glEnd();
    
    glPopMatrix();
}

void OpenGLRenderer::DrawCircle(float centerX, float centerY, float radius, int segments)
{
    glPushMatrix();
    glTranslatef(m_transformX, m_transformY, 0.0f);
    glRotatef(m_rotation, 0.0f, 0.0f, 1.0f);
    glScalef(m_scale, m_scale, 1.0f);
    
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < segments; ++i)
    {
        float angle = 2.0f * 3.14159265359f * i / segments;
        float x = centerX + cosf(angle) * radius;
        float y = centerY + sinf(angle) * radius;
        glVertex2f(x, y);
    }
    glEnd();
    
    glPopMatrix();
}

void OpenGLRenderer::SetTransform(float x, float y, float rotation, float scale)
{
    m_transformX = x;
    m_transformY = y;
    m_rotation = rotation;
    m_scale = scale;
}

unsigned int OpenGLRenderer::LoadTexture(const std::string& filename)
{
    // 这里应该实现纹理加载逻辑
    // 为了简化，我们返回0表示未实现
    return 0;
}

void OpenGLRenderer::UseTexture(unsigned int textureId)
{
    if (textureId != 0)
    {
        glBindTexture(GL_TEXTURE_2D, textureId);
        glEnable(GL_TEXTURE_2D);
    }
    else
    {
        glDisable(GL_TEXTURE_2D);
    }
}