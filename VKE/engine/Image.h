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
        Image(Device& device,const std::string& filepath);
        ~Image();

        void CreateTextureImage(const std::string& filename);
        void CreateTextureImageView();
        void CreateTextureSampler();

      

        VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);


        VkImageView GetTextureImageView() const { return textureImageView; }
        VkSampler GetTextureSampler() const { return textureSampler; }

        void Cleanup();

    private:

        void CreateImage(const Device& device,
                         uint32_t width,
                         uint32_t height,
                         VkFormat format, VkImageTiling tiling,
                         VkImageUsageFlags usage,
                         VkMemoryPropertyFlags properties,
                         VkImage& image, VkDeviceMemory&
                         imageMemory);

        void TransitionImageLayout(VkImage image,
                                   VkFormat format,
                                   VkImageLayout oldLayout,
                                   VkImageLayout newLayout);

        Device& m_Device;

        VkImage textureImage{};
        VkDeviceMemory textureImageMemory{};
        VkImageView textureImageView{};
        VkSampler textureSampler{};
    
    };
}
