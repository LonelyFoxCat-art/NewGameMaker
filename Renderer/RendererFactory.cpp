#include "RendererFactory.h"
#include "OpenGLRenderer.h"
#include "DirectXRenderer.h"
#include "VulkanRenderer.h"
#include "SoftwareRenderer.h"

IRenderer* RendererFactory::CreateRenderer(RendererType type)
{
    switch (type)
    {
        case RendererType::OpenGL:
            return new OpenGLRenderer();
        case RendererType::DirectX:
            return new DirectXRenderer();
        case RendererType::Vulkan:
            return new VulkanRenderer();
        case RendererType::Software:
            return new SoftwareRenderer();
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