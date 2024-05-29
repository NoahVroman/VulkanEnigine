#pragma once

#include <vulkan/vulkan.h>
#include <string>
#include <cassert>
#include "Buffer.h"

#include <memory>

namespace Engine
{
    class Image final
    {
    public:
        void CreateTexture(const std::string& texturePath, Device device, const VkPhysicalDevice& physicalDevice, VkQueue graphicsQueue);

        VkImageView GetTextureImageView() const { return m_TextureImageView; }
        VkSampler GetTextureSampler() const { return m_TextureSampler; }

    private:
        void CreateTextureImage(const std::string& texturePath, Device& device, VkPhysicalDevice physicalDevice, VkQueue graphicsQueue);
        void TransitionImageLayout(const Device& device, VkQueue graphicsQueue, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
        void CopyBufferToImage(const Device& device, VkQueue graphicsQueue, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
        static VkCommandBuffer BeginSingleTimeCommands(const Device& device);
        static void EndSingleTimeCommands(const Device& device, VkCommandBuffer commandBuffer, VkQueue graphicsQueue);
        void CreateTextureImageView(const Device& device);
        void CreateImageSampler(const Device& device, const VkPhysicalDevice& physicalDevice);

        VkImageView CreateImageVieuw(const Device& device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, VkImageView& imageView);
        void CreateImage(const Device& device, const VkPhysicalDevice& physicalDevice, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);

        VkImage m_TextureImage{};
        VkDeviceMemory m_TextureImageMemory{};
        VkImageView m_TextureImageView{};
        VkSampler m_TextureSampler{};

        // Add a Buffer member variable
        std::unique_ptr<Buffer> m_StagingBuffer{};
    };
}
