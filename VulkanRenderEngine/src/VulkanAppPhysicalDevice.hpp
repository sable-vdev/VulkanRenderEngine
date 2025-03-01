#pragma once
#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include <iostream>
#include <stdexcept>
#include <vector>
#include <string>
#include <map>
#include <set>
#include "VulkanAppQueueFamilies.hpp"

class VulkanAppPhysicalDevice
{
public:
	VkPhysicalDevice GetPhysicalDevices(VkInstance instance, VkSurfaceKHR surface);
private:
	VkPhysicalDevice UserSelectGPU(std::vector<VkPhysicalDevice> devices, uint32_t* deviceCount, VkSurfaceKHR surface);
	VkPhysicalDevice GetBestGPU(std::vector<VkPhysicalDevice> devices, VkSurfaceKHR surface);
	bool CheckDeviceExtensionSupport(VkPhysicalDevice vkpd);
	std::string GetVulkanDeviceInfo(VkPhysicalDevice vkpd);
	int CalculateDeviceScore(VkPhysicalDevice vkpd);
};

