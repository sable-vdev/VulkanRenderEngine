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
	void CreateGraphicsPipeline();
	VkShaderModule CreateShaderModule(const std::vector<char>& code);

	inline std::vector<char> ReadFile(const std::string& filename)
	{
		std::ifstream file(filename, std::ios::ate | std::ios::binary);

		if (!file.is_open())
		{
			throw std::runtime_error("Failed to open file");
		}

		size_t fileSize = static_cast<size_t>(file.tellg());
		std::vector<char> buffer(fileSize);
		file.seekg(0);
		file.read(buffer.data(), fileSize);
		file.close();

		return buffer;
	}
};

