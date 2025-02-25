#pragma once
#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include <stdexcept>
#include <vector>
#include <iostream>
#include "VulkanAppDebugger.hpp"
#include "VulkanAppPhysicalDevice.hpp"
#include "VulkanAppQueueFamilies.hpp"

class VulkanApp
{
private:
	VkDevice device = nullptr;
	VkPhysicalDevice mPhysicalDevice = nullptr;
	VulkanAppDebugger mDebugger;
	GLFWwindow* mWindow = nullptr;
	VkInstance mInstance = nullptr;
	const uint32_t WIDTH = 1280;
	const uint32_t HEIGHT = 720;
	VkDebugUtilsMessengerEXT mDebugMessenger = nullptr;
public:
	void Run();
private:
	void InitGLFW();
	void InitVulkan();
	void MainLoop();
	void CleanUp();
	void CreateVulkanInstance();
	void GetPhysicalDevices();
	void CreateLogicalDevice();
	std::vector<const char*> GetRequiredExtensions();
	void SetupDebugMessenger();
};

