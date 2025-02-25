#include "VulkanAppPhysicalDevice.hpp"

const bool userSelectDevice = false;

VkPhysicalDevice VulkanAppPhysicalDevice::GetPhysicalDevices(VkInstance instance)
{
	VkPhysicalDevice physicalDevice = nullptr;
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

	if (deviceCount <= 0) throw std::runtime_error("Failed to find any GPU supporting Vulkan!");

	if (userSelectDevice)
	{
		physicalDevice = UserSelectGPU(devices, &deviceCount);
	}
	else
	{
		physicalDevice = GetBestGPU(devices);
		std::cout << "Using: " << GetVulkanDeviceInfo(physicalDevice) << '\n';
	}

	if (physicalDevice == nullptr) throw std::runtime_error("Failed to find any GPU supporting Vulkan!");

	return physicalDevice;
}

VkPhysicalDevice VulkanAppPhysicalDevice::GetBestGPU(std::vector<VkPhysicalDevice> devices)
{
	int deviceRank = 0;
	std::multimap<int, VkPhysicalDevice> vulkanDevices;

	for (const auto& device : devices)
	{
		VulkanAppQueueFamilies family = family.FindQueueFamilies(device);

		if (family.IsComplete())
		{
			int score = CalculateDeviceScore(device);
			vulkanDevices.insert(std::make_pair(score, device));
		}
	}

	if (vulkanDevices.rbegin()->first > 0)
	{
		return vulkanDevices.rbegin()->second;
	}
	else throw std::runtime_error("Failed to find suitable GPU");

}

VkPhysicalDevice VulkanAppPhysicalDevice::UserSelectGPU(std::vector<VkPhysicalDevice> devices, uint32_t* deviceCount)
{
	int deviceNumber = 0;
	std::map<int, VkPhysicalDevice> vulkanDevices;

	for (const auto& device : devices)
	{
		VulkanAppQueueFamilies family = family.FindQueueFamilies(device);
		if (family.IsComplete())
		{
			std::cout << GetVulkanDeviceInfo(device) << " at index: " << deviceNumber << '\n';
			vulkanDevices.insert(std::make_pair(deviceNumber, device));
			deviceNumber++;
		}
	}

	std::cout << "Choose the GPU you want to use:\n";
	uint8_t input;
	std::cin >> input;

	if (input >= *deviceCount) throw std::runtime_error("You selected a wrong index");

	return vulkanDevices.at(input);
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