#pragma once
#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include "VulkanAppQueueFamilies.hpp"
#include <vector>
#include <set>
#include <stdexcept>

class VulkanAppLogicalDevice
{
public:
	VkDevice vulkanDevice = nullptr;
private:
	VkQueue mGraphicsQueue = nullptr;
	VkQueue mPresentQueue = nullptr;
public:
	VulkanAppLogicalDevice();
	VulkanAppLogicalDevice(VulkanAppQueueFamilies family, VkPhysicalDevice physicalDevice, bool enableValidationLayers, const std::vector<const char*>& deviceExtensions, const std::vector<const char*>& validationLayers);
	void DestoryLogicalDevice();
};

