#pragma once
#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include <iostream>
#include <stdexcept>
#include <vector>
#include <string>
#include <map>
#include <set>
#include "VulkanAppSwapChain.hpp"
#include "VulkanAppQueueFamilies.hpp"

const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

class VulkanAppPhysicalDevice
{
public:
	VkPhysicalDevice GetPhysicalDevices(VkInstance instance, VkSurfaceKHR surface);
private:
	VkPhysicalDevice GetBestGPU(const std::vector<VkPhysicalDevice>& devices, VkSurfaceKHR surface);
	bool CheckDeviceExtensionSupport(VkPhysicalDevice vkpd);
	std::string GetVulkanDeviceInfo(VkPhysicalDevice vkpd);
	int CalculateDeviceScore(VkPhysicalDevice vkpd);
};