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

class VulkanApp
{
private:
	VulkanAppLogicalDevice m_vulkanAppLogicalDevice;
	VulkanAppPhysicalDevice m_vulkanAppPhysicalDevice;
	VulkanAppDebugger m_debugger;
	GLFWwindow* m_Window = nullptr;
	VkInstance m_Instance = nullptr;
	const int WIDTH = 1280;
	const int HEIGHT = 720;
	const char* TITLE = "Hello Vulkan";
	VkSurfaceKHR m_surfaceKhr = nullptr;
	VulkanAppSwapChain m_vulkanAppSwapChain;
	VulkanAppImageView m_vulkanAppImageView;
	VkPipelineLayout m_pipelineLayout;
public:
	VulkanApp() = default;
	void Run();
private:
	void InitGlfw();
	void InitVulkan();
	void MainLoop();
	void CleanUp();
	void CreateVulkanInstance();
	void CreateLogicalDevice();
	std::vector<const char*> GetRequiredExtensions();
	void SetupDebugMessenger();
	void CreateSurfaceGlfw(); //Implementation can be made platform specific without glfw using win32api
	void CreateSwapChain();
	void CreateGraphicsPipeline();
	VkShaderModule CreateShaderModule(const std::vector<char>& code);

	static std::vector<char> ReadFile(const std::string& filename)
	{
		std::ifstream file(filename, std::ios::ate | std::ios::binary);

		if (!file.is_open())
		{
			throw std::runtime_error("Failed to open file");
		}

		const size_t fileSize = file.tellg();
		std::vector<char> buffer(fileSize);
		file.seekg(0);
		file.read(buffer.data(), fileSize);
		file.close();

		return buffer;
	}
};

