#pragma once
#include "IRenderer.h"
#include <vulkan/vulkan.h>
#include <vector>
#include <unordered_map>
#include <optional>

// Helper structures
struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool IsComplete() {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

class VulkanRenderer : public IRenderer
{
public:
    VulkanRenderer();
    virtual ~VulkanRenderer();

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

private:
    // Vulkan objects
    VkInstance instance;
    VkPhysicalDevice physicalDevice;
    VkDevice logicalDevice;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    VkSurfaceKHR surface;
    VkSwapchainKHR swapchain;
    std::vector<VkImage> swapchainImages;
    std::vector<VkImageView> swapchainImageViews;
    VkFormat swapchainImageFormat;
    VkExtent2D swapchainExtent;
    VkRenderPass renderPass;
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;
    std::vector<VkFramebuffer> swapchainFramebuffers;
    VkCommandPool commandPool;
    std::vector<VkCommandBuffer> commandBuffers;
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    size_t currentFrame;

    // Window handle
    HWND windowHandle;

    // Rendering state
    float clearColor[4];
    float transform[4]; // x, y, rotation, scale

    // Texture management
    std::unordered_map<unsigned int, VkImage> textures;
    unsigned int nextTextureId;

    // Helper methods
    bool CreateInstance();
    bool CreateSurface();
    bool PickPhysicalDevice();
    bool CreateLogicalDevice();
    bool CreateSwapchain();
    bool CreateImageViews();
    bool CreateRenderPass();
    bool CreateGraphicsPipeline();
    bool CreateFramebuffers();
    bool CreateCommandPool();
    bool CreateCommandBuffers();
    bool CreateSyncObjects();
    VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
    uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, 
                     VkBuffer& buffer, VkDeviceMemory& bufferMemory);
    void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
};