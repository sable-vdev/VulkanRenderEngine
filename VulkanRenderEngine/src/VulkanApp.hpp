#pragma once
#include <stdexcept>
#include <vector>
#include <set>
#include <iostream>
#include <cstring>
#include <fstream>
#include "VulkanAppDebugger.hpp"
#include "VulkanAppPhysicalDevice.hpp"
#include "VulkanAppLogicalDevice.hpp"
#include "VulkanAppImageView.hpp"
#include "VulkanAppGraphicsPipeline.hpp"

class VulkanApp
{
private:
	VulkanAppLogicalDevice m_vulkanAppLogicalDevice;
	VulkanAppPhysicalDevice m_vulkanAppPhysicalDevice;
	VulkanAppDebugger m_debugger;
	GLFWwindow* m_window = nullptr;
	VkInstance m_instance = nullptr;
	const int WIDTH = 1280;
	const int HEIGHT = 720;
	const char* TITLE = "Hello Vulkan";
	VkSurfaceKHR m_surfaceKhr = nullptr;
	VulkanAppSwapChain m_vulkanAppSwapChain;
	VulkanAppImageView m_vulkanAppImageView;
	VulkanAppGraphicsPipeline m_vulkanAppGraphicsPipeline;
public:
	VulkanApp() = default;
	void Run();
private:
	void InitGlfw();
	void InitVulkan();
	void MainLoop() const;
	void CleanUp();
	void CreateVulkanInstance();
	void CreateLogicalDevice();
	std::vector<const char*> GetRequiredExtensions();
	void SetupDebugMessenger();
	void CreateSurfaceGlfw(); //Implementation can be made platform specific without glfw using win32api
	void CreateSwapChain();
};

