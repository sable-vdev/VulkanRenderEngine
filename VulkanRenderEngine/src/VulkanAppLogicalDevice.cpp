#include "VulkanAppLogicalDevice.hpp"

VulkanAppLogicalDevice::VulkanAppLogicalDevice(VulkanAppQueueFamilies family, VkPhysicalDevice physicalDevice, bool enableValidationLayers, const std::vector<const char*>& deviceExtensions, const std::vector<const char*>& validationLayers)
{

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos{};
	
	if (!family.graphicsFamily.has_value() || !family.presentFamily.has_value())
	{
		throw std::runtime_error("Graphics Family or Present Family not found");
	}
	
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

	if (vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &vulkanDevice) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create logical device");
	}
	
	if (!family.graphicsFamily.has_value() || !family.presentFamily.has_value())
	{
		throw std::runtime_error("Graphics Family or Present Family not found");
	}
	
	vkGetDeviceQueue(vulkanDevice, family.graphicsFamily.value(), 0, &mGraphicsQueue);
	vkGetDeviceQueue(vulkanDevice, family.presentFamily.value(), 0, &mPresentQueue);
}

void VulkanAppLogicalDevice::DestroyLogicalDevice() const
{
	vkDestroyDevice(vulkanDevice, nullptr);
}
