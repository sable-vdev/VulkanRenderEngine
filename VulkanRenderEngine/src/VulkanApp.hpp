#pragma once
#include <stdexcept>
#include <vector>
#include <set>
#include <iostream>
#include <cstring>
#include "VulkanAppDebugger.hpp"
#include "VulkanAppPhysicalDevice.hpp"
#include "VulkanAppLogicalDevice.hpp"
//#include "VulkanAppSwapChain.hpp"
#include "VulkanAppImageView.hpp"

class VulkanApp
{
private:
	VulkanAppLogicalDevice mVulkanAppLogicalDevice;
	VulkanAppPhysicalDevice mVulkanAppPhysicalDevice{};
	VulkanAppDebugger mDebugger;
	GLFWwindow* mWindow = nullptr;
	VkInstance mInstance = nullptr;
	const uint32_t WIDTH = 1280;
	const uint32_t HEIGHT = 720;
	VkSurfaceKHR mSurfaceKHR = nullptr;
	VulkanAppSwapChain mVulkanAppSwapChain;
	VulkanAppImageView mVulkanAppImageView;
public:
	void Run();
private:
	void InitGLFW();
	void InitVulkan();
	void MainLoop();
	void CleanUp();
	void CreateVulkanInstance();
	void CreateLogicalDevice();
	std::vector<const char*> GetRequiredExtensions();
	void SetupDebugMessenger();
	void CreateSurfaceGLFW(); //Implementation can be made platform specific without glfw using win32api
	void CreateSwapChain();
};

