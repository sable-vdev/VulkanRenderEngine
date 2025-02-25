#pragma once
#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include <iostream>
#include <stdexcept>
#include <vector>
#include <string>
#include <map>
#include "VulkanAppQueueFamilies.hpp"

class VulkanAppPhysicalDevice
{
public:
	VkPhysicalDevice GetPhysicalDevices(VkInstance instance);
private:
	VkPhysicalDevice UserSelectGPU(std::vector<VkPhysicalDevice> devices, uint32_t* deviceCount);
	VkPhysicalDevice GetBestGPU(std::vector<VkPhysicalDevice> devices);
	std::string GetVulkanDeviceInfo(VkPhysicalDevice device);
	int CalculateDeviceScore(VkPhysicalDevice vkpd);
};

