#pragma once
#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include <vector>
#include <stdexcept>

class Framebuffer
{
public:
	std::vector<VkFramebuffer> frameBuffers;
public:
	void CreateFramebuffers(VkDevice& device, std::vector<VkImageView>& imageViews, VkExtent2D swapChainExtent, VkRenderPass& renderPass);
	void DestroyFramebuffers(VkDevice& device);
};

