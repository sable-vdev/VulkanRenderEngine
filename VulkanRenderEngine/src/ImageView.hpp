#pragma once
#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include <stdexcept>
#include "SwapChain.hpp"
#include <vector>

class ImageView
{
public:
	std::vector<VkImageView> imageViews;
public:
	void CreateImageViews(const VkDevice& device, const SwapChain& swapChain);
	void DestroyImageViews(const VkDevice& device) const;
};

