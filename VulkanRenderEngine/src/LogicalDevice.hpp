#pragma once
#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include "QueueFamilies.hpp"
#include <vector>
#include <set>
#include <stdexcept>

class LogicalDevice
{
public:
	VkDevice vulkanDevice = nullptr;
private:
	VkQueue mGraphicsQueue = nullptr;
	VkQueue mPresentQueue = nullptr;
public:
	LogicalDevice() = default;
	LogicalDevice(QueueFamilies family, VkPhysicalDevice physicalDevice, bool enableValidationLayers, const std::vector<const char*>& deviceExtensions, const std::vector<const char*>& validationLayers);
	void DestroyLogicalDevice() const;
};

