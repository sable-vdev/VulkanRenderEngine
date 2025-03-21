#include "PhysicalDevice.hpp"

void PhysicalDevice::GetPhysicalDevice(const VkInstance instance, const VkSurfaceKHR surface)
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

	if (deviceCount <= 0) throw std::runtime_error("Failed to find any GPU supporting Vulkan!");
	const VkPhysicalDevice physicalDevice = GetBestGPU(devices, surface);
	std::cout << "Using: " << GetVulkanDeviceInfo(physicalDevice) << '\n';

	if (physicalDevice == nullptr) throw std::runtime_error("Failed to find any GPU supporting Vulkan!");

	vulkanAppPhysicalDevice = physicalDevice;
}

VkPhysicalDevice PhysicalDevice::GetBestGPU(const std::vector<VkPhysicalDevice>& devices, VkSurfaceKHR surface)
{
	std::multimap<int, VkPhysicalDevice> vulkanDevices;

		
	for (const auto& device : devices)
	{
		QueueFamilies family;
		family = family.FindQueueFamilies(device, surface);
		if (CheckDeviceExtensionSupport(device))
		{
			SwapChain swapchain;
			SwapChainSupportDetails swapChainDetails = swapchain.QuerySwapChainSupport(device, surface);
			if (!swapChainDetails.formats.empty() && !swapChainDetails.presentMode.empty())
			{
				int score = CalculateDeviceScore(device);
				vulkanDevices.insert(std::make_pair(score, device));
			}
		}
	}

	if (!vulkanDevices.empty() && vulkanDevices.rbegin()->first > 0)
	{
		return vulkanDevices.rbegin()->second;
	}
	throw std::runtime_error("Failed to find suitable GPU");
}

bool PhysicalDevice::CheckDeviceExtensionSupport(VkPhysicalDevice vkpd)
{
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(vkpd, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(vkpd, nullptr, &extensionCount, availableExtensions.data());

	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

	for (const auto& extension : availableExtensions)
	{
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

std::string PhysicalDevice::GetVulkanDeviceInfo(VkPhysicalDevice vkpd)
{
	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(vkpd, &deviceProperties);
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceFeatures(vkpd, &deviceFeatures);
	std::string deviceName;
	std::string deviceType;

	for (const char i : deviceProperties.deviceName)
	{
		if (i == '\0') break;
		deviceName += i;
	}

	switch (deviceProperties.deviceType)
	{
	case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
		deviceType = "Integrated GPU";
		break;
	case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
		deviceType = "Discrete GPU";
		break;
	default:
		deviceType = "Devicetype unkown " + std::to_string(deviceProperties.deviceType);
		break;
	}

	return deviceName + ", " + deviceType;
}

int PhysicalDevice::CalculateDeviceScore(VkPhysicalDevice vkpd)
{
	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(vkpd, &deviceProperties);
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceFeatures(vkpd, &deviceFeatures);

	int score = 0;

	if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
	{
		score += 100;
	}

	score += deviceProperties.limits.maxImageDimension2D;

	if (!deviceFeatures.geometryShader) return 0;

	return score;
}