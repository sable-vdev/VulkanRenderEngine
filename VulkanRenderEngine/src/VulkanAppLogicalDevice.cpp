#include "VulkanAppLogicalDevice.hpp"

VulkanAppLogicalDevice::VulkanAppLogicalDevice()
{
}

VulkanAppLogicalDevice::VulkanAppLogicalDevice(VulkanAppQueueFamilies family, VkPhysicalDevice physicalDevice, bool enableValidationLayers, const std::vector<const char*>& deviceExtensions, const std::vector<const char*>& validationLayers)
{
	//VulkanAppQueueFamilies family = family.FindQueueFamilies(mVulkanAppPhysicalDevice.vulkanAppPhysicalDevice, mSurfaceKHR);

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos{};
	std::set<uint32_t> uniqueQueueFamilies = { family.graphicsFamily.value(), family.presentFamily.value() };

	float queuePriority = 1.0f;

	for (uint32_t queueFamily : uniqueQueueFamilies)
	{
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures{};

	VkDeviceCreateInfo deviceCreateInfo{};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
	deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
	deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();

	if (enableValidationLayers)
	{
		deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		deviceCreateInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else deviceCreateInfo.enabledLayerCount = 0;
	/*
	if (vkCreateDevice(mPhysicalDevice, &deviceCreateInfo, nullptr, &vulkanDevice) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create logical device");
	}
	*/
	if (vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &vulkanDevice) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create logical device");
	}

	vkGetDeviceQueue(vulkanDevice, family.graphicsFamily.value(), 0, &mGraphicsQueue);
	vkGetDeviceQueue(vulkanDevice, family.presentFamily.value(), 0, &mPresentQueue);
}

void VulkanAppLogicalDevice::DestoryLogicalDevice()
{
	vkDestroyDevice(vulkanDevice, nullptr);
}
