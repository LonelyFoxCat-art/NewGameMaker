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
    // Generate texture ID
    unsigned int textureId;
    glGenTextures(1, &textureId);
    
    // In a real implementation, we would:
    // 1. Load the image file using a library like SOIL, STB, or FreeImage
    // 2. Get image data (width, height, channels, etc.)
    // 3. Bind the texture and upload the image data to GPU
    
    // For this implementation, we'll create a placeholder texture
    glBindTexture(GL_TEXTURE_2D, textureId);
    
    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // Create placeholder texture data (a simple 2x2 pixel pattern)
    unsigned char placeholderData[] = {
        255, 0, 0, 255,     // Red pixel
        0, 255, 0, 255,     // Green pixel
        0, 0, 255, 255,     // Blue pixel
        255, 255, 0, 255    // Yellow pixel
    };
    
    // Upload placeholder texture data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, placeholderData);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    return textureId;
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

unsigned int OpenGLRenderer::LoadShader(const std::string& vertexShaderFile, const std::string& fragmentShaderFile)
{
    // Read shader files
    std::string vertexCode = ReadShaderFile(vertexShaderFile);
    std::string fragmentCode = ReadShaderFile(fragmentShaderFile);
    
    if (vertexCode.empty() || fragmentCode.empty()) {
        // If file reading fails, use default shaders
        vertexCode = R"(
            #version 330 core
            layout (location = 0) in vec3 aPos;
            layout (location = 1) in vec2 aTexCoord;
            
            out vec2 TexCoord;
            
            uniform mat4 transform;
            
            void main()
            {
                gl_Position = transform * vec4(aPos, 1.0);
                TexCoord = aTexCoord;
            }
        )";
        
        fragmentCode = R"(
            #version 330 core
            out vec4 FragColor;
            
            in vec2 TexCoord;
            
            uniform sampler2D ourTexture;
            
            void main()
            {
                FragColor = texture(ourTexture, TexCoord);
            }
        )";
    }
    
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();
    
    // Compile vertex shader
    unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    
    // Check for vertex shader compilation errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        OutputDebugStringA(("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" + std::string(infoLog)).c_str());
    }
    
    // Compile fragment shader
    unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    
    // Check for fragment shader compilation errors
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        OutputDebugStringA(("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" + std::string(infoLog)).c_str());
    }
    
    // Create shader program
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertex);
    glAttachShader(shaderProgram, fragment);
    glLinkProgram(shaderProgram);
    
    // Check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        OutputDebugStringA(("ERROR::SHADER::PROGRAM::LINKING_FAILED\n" + std::string(infoLog)).c_str());
    }
    
    // Delete shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    
    return shaderProgram;
}

void OpenGLRenderer::UseShader(unsigned int shaderId)
{
    // In a real implementation, this would activate the shader program
    if (shaderId != 0) {
        glUseProgram(shaderId);
    } else {
        glUseProgram(0);  // Use fixed function pipeline or default shader
    }
}

void OpenGLRenderer::SetSurface(unsigned int width, unsigned int height)
{
    // 在实际实现中，这里需要调整渲染表面大小
    // 例如重新配置视口、投影矩阵等
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, (GLdouble)width, (GLdouble)height, 0.0);
    glMatrixMode(GL_MODELVIEW);
}

std::string OpenGLRenderer::ReadShaderFile(const std::string& filePath)
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