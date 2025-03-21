#pragma once
#include <iostream>
#include <stdexcept>
#include <vector>
#include <string>
#include <map>
#include <set>
#include "SwapChain.hpp"
#include "QueueFamilies.hpp"

const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

class PhysicalDevice
{
public:
	VkPhysicalDevice vulkanAppPhysicalDevice;
	void GetPhysicalDevice(VkInstance instance, VkSurfaceKHR surface);
private:
	VkPhysicalDevice GetBestGPU(const std::vector<VkPhysicalDevice>& devices, VkSurfaceKHR surface);
	bool CheckDeviceExtensionSupport(VkPhysicalDevice vkpd);
	std::string GetVulkanDeviceInfo(VkPhysicalDevice vkpd);
	int CalculateDeviceScore(VkPhysicalDevice vkpd);
};