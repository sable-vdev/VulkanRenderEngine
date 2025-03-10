#include "VulkanAppPhysicalDevice.hpp"
/*
VkPhysicalDevice VulkanAppPhysicalDevice::GetPhysicalDevice(VkInstance instance, VkSurfaceKHR surface)
{
	VkPhysicalDevice physicalDevice = nullptr;
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

	if (deviceCount <= 0) throw std::runtime_error("Failed to find any GPU supporting Vulkan!");
	physicalDevice = GetBestGPU(devices, surface);
	std::cout << "Using: " << GetVulkanDeviceInfo(physicalDevice) << '\n';

	if (physicalDevice == nullptr) throw std::runtime_error("Failed to find any GPU supporting Vulkan!");

	return physicalDevice;
}
*/

void VulkanAppPhysicalDevice::GetPhysicalDevice(VkInstance instance, VkSurfaceKHR surface)
{
	VkPhysicalDevice physicalDevice = nullptr;
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

	if (deviceCount <= 0) throw std::runtime_error("Failed to find any GPU supporting Vulkan!");
	physicalDevice = GetBestGPU(devices, surface);
	std::cout << "Using: " << GetVulkanDeviceInfo(physicalDevice) << '\n';

	if (physicalDevice == nullptr) throw std::runtime_error("Failed to find any GPU supporting Vulkan!");

	vulkanAppPhysicalDevice = physicalDevice;
}

VkPhysicalDevice VulkanAppPhysicalDevice::GetBestGPU(const std::vector<VkPhysicalDevice>& devices, VkSurfaceKHR surface)
{
	int deviceRank = 0;
	std::multimap<int, VkPhysicalDevice> vulkanDevices;

		
	for (const auto& device : devices)
	{
		VulkanAppQueueFamilies family = family.FindQueueFamilies(device, surface);
		if (CheckDeviceExtensionSupport(device))
		{
			VulkanAppSwapChain swapchain;
			//SwapChainSupportDetails swapChainDetails = QuerySwapChainSupport(device, surface);
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
	else throw std::runtime_error("Failed to find suitable GPU");
}

bool VulkanAppPhysicalDevice::CheckDeviceExtensionSupport(VkPhysicalDevice vkpd)
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

std::string VulkanAppPhysicalDevice::GetVulkanDeviceInfo(VkPhysicalDevice device)
{
	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
	std::string deviceName = "";
	std::string deviceType = "";

	for (unsigned int i = 0; i < 256; i++)
	{
		if (deviceProperties.deviceName[i] == '\0') break;
		deviceName += deviceProperties.deviceName[i];
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
		deviceType = "Devicetype unkown";
		break;
	}

	return deviceName + ", " + deviceType;
}

int VulkanAppPhysicalDevice::CalculateDeviceScore(VkPhysicalDevice vkpd)
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