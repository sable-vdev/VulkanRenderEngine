#pragma once
#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include <stdexcept>
#include <vector>
#include <set>
#include <iostream>
#include <cstring>
#include "VulkanAppDebugger.hpp"
#include "VulkanAppPhysicalDevice.hpp"
#include "VulkanAppQueueFamilies.hpp"
#include "VulkanAppSwapChain.hpp"



class VulkanApp
{
private:
	VkDevice mDevice = nullptr;
	VkQueue mGraphicsQueue = nullptr;
	VkQueue mPresentQueue = nullptr;
	VkPhysicalDevice mPhysicalDevice = nullptr;
	VulkanAppDebugger mDebugger;
	GLFWwindow* mWindow = nullptr;
	VkInstance mInstance = nullptr;
	const uint32_t WIDTH = 1280;
	const uint32_t HEIGHT = 720;
	VkDebugUtilsMessengerEXT mDebugMessenger = nullptr;
	VkSurfaceKHR mSurfaceKHR = nullptr;
	VkSwapchainKHR mSwapChain = nullptr;
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
	void CreateSurfaceGLFW(); //Implementation can be made platform specific without glfw using win32api
	void CreateSwapChain();
};

