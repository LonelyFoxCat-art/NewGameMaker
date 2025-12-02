#include "RendererFactory.h"
#include "OpenGLRenderer.h"
#include "DirectXRenderer.h"

IRenderer* RendererFactory::CreateRenderer(RendererType type)
{
    switch (type)
    {
        case RendererType::OpenGL:
            return new OpenGLRenderer();
        case RendererType::DirectX:
            return new DirectXRenderer();
        default:
            return nullptr;
    }
}

void RendererFactory::DestroyRenderer(IRenderer* renderer)
{
    if (renderer)
    {
        delete renderer;
    }
}