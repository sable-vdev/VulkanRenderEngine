#pragma once
#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include <inttypes.h>
#include <optional>
#include <vector>

struct VulkanAppQueueFamilies
{
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;
	bool IsComplete()
	{
		return graphicsFamily.has_value() && presentFamily.has_value();
	}

	VulkanAppQueueFamilies FindQueueFamilies(VkPhysicalDevice vkpd, VkSurfaceKHR surface)
	{
		VulkanAppQueueFamilies queueFamilies;

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(vkpd, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamiliesVec(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(vkpd, &queueFamilyCount, queueFamiliesVec.data());

		int i = 0;
		for (const auto& queueFamily : queueFamiliesVec)
		{
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				queueFamilies.graphicsFamily = i;
			}

			uint32_t presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(vkpd, i, surface, &presentSupport);

			if (presentSupport) queueFamilies.presentFamily = i;

			if (queueFamilies.IsComplete()) break;

			i++;
		}

		return queueFamilies;
	}

	VulkanAppQueueFamilies FindQueueFamilies(VkPhysicalDevice vkpd)
	{
		VulkanAppQueueFamilies queueFamilies;

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(vkpd, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamiliesVec(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(vkpd, &queueFamilyCount, queueFamiliesVec.data());

		int i = 0;
		for (const auto& queueFamily : queueFamiliesVec)
		{
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				queueFamilies.graphicsFamily = i;
			}
			if (queueFamilies.IsComplete()) break;

			i++;
		}

		return queueFamilies;
	}
};

