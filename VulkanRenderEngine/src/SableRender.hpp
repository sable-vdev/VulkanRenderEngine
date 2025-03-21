#pragma once
#include <set>
#include <iostream>
#include <cstring>
#include "Debugger.hpp"
#include "PhysicalDevice.hpp"
#include "LogicalDevice.hpp"
#include "ImageView.hpp"
#include "GraphicsPipeline.hpp"
#include "Framebuffer.hpp"
#include "CommandBuffer.hpp"

class SableRender
{
private:
	const int WIDTH = 1280;
	const int HEIGHT = 720;
	const char* TITLE = "Hello Vulkan";
	float m_lastDeltaTime = 0.0;
	float m_frameCount = 0.0f;
	GLFWwindow* m_window = nullptr;
	VkInstance m_instance = nullptr;
	VkSurfaceKHR m_surfaceKhr = nullptr;
	LogicalDevice m_sableLogicalDevice;
	PhysicalDevice m_sablePhysicalDevice;
	Debugger m_debugger;
	SwapChain m_sableSwapChain;
	ImageView m_sableImageView;
	GraphicsPipeline m_sableGraphicsPipeline;
	Framebuffer m_sableFramebuffer;
	CommandBuffer m_sableCommandBuffer;


	VkSemaphore m_imageAvailableSemaphore;
	VkSemaphore m_renderFinishedSemaphore;
	VkFence m_inFlightFence;
public:
	SableRender() = default;
	void Run();
private:
	void InitGlfw();
	void InitVulkan();
	void MainLoop();
	void DrawFrame();
	void CalculateFps();
	void CleanUp();
	void CreateVulkanInstance();
	void CreateLogicalDevice();
	std::vector<const char*> GetRequiredExtensions();
	void SetupDebugMessenger();
	void CreateSurfaceGlfw(); //Implementation can be made platform specific without glfw using win32api
	void CreatePhysicalDevice();
	void CreateSwapChain();
	void CreateImageViews();
	void CreateGraphicsPipeline();
	void CreateFramebuffer();
	void CreateCommandBuffer();
	void CreateSyncObjects();
};

