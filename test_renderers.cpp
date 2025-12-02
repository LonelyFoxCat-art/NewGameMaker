#include "Renderer/RendererFactory.h"
#include "Renderer/IRenderer.h"
#include <iostream>

int main()
{
    std::cout << "Testing Renderer Creation..." << std::endl;
    
    // Test Vulkan Renderer
    IRenderer* vulkanRenderer = RendererFactory::CreateRenderer(RendererType::Vulkan);
    if (vulkanRenderer) {
        std::cout << "✓ Vulkan Renderer created successfully" << std::endl;
        RendererFactory::DestroyRenderer(vulkanRenderer);
    } else {
        std::cout << "✗ Failed to create Vulkan Renderer" << std::endl;
    }
    
    // Test Software Renderer
    IRenderer* softwareRenderer = RendererFactory::CreateRenderer(RendererType::Software);
    if (softwareRenderer) {
        std::cout << "✓ Software Renderer created successfully" << std::endl;
        RendererFactory::DestroyRenderer(softwareRenderer);
    } else {
        std::cout << "✗ Failed to create Software Renderer" << std::endl;
    }
    
    // Test OpenGL Renderer
    IRenderer* openGLRenderer = RendererFactory::CreateRenderer(RendererType::OpenGL);
    if (openGLRenderer) {
        std::cout << "✓ OpenGL Renderer created successfully" << std::endl;
        RendererFactory::DestroyRenderer(openGLRenderer);
    } else {
        std::cout << "✗ Failed to create OpenGL Renderer" << std::endl;
    }
    
    // Test DirectX Renderer
    IRenderer* directXRenderer = RendererFactory::CreateRenderer(RendererType::DirectX);
    if (directXRenderer) {
        std::cout << "✓ DirectX Renderer created successfully" << std::endl;
        RendererFactory::DestroyRenderer(directXRenderer);
    } else {
        std::cout << "✗ Failed to create DirectX Renderer" << std::endl;
    }
    
    std::cout << "All renderer tests completed!" << std::endl;
    return 0;
}