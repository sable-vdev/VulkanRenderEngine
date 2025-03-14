#pragma once
#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include <stdexcept>
#include "VulkanAppSwapChain.hpp"
#include <vector>

class VulkanAppImageView
{
public:
	std::vector<VkImageView> imageViews;
public:
	void CreateImageViews(const VkDevice& device, const VulkanAppSwapChain& swapChain);
	void DestroyImageViews(const VkDevice& device) const;
};

