#pragma once
#include <inttypes.h>
#include <optional>

struct VulkanAppQueueFamilies
{
	std::optional<uint32_t> graphicsFamily;

	bool IsComplete()
	{
		return graphicsFamily.has_value();
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

