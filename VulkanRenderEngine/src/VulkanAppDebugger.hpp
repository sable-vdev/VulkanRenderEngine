#pragma once
#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include <iostream>

enum LogLevel : int8_t
{
	INFO,
	VERBOSE,
	WARNING,
	ERROR
};

class VulkanAppDebugger
{
private:
	VkDebugUtilsMessengerEXT mDebugMessenger = nullptr;
	VkInstance mInstance = nullptr;
private:
	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
	VkResult CreateDebugUtilsMessengerExt(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
public:
	void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	void DestroyDebugUtilsMessengerEXT(const VkAllocationCallbacks* pAllocator = nullptr);
	VulkanAppDebugger(VkInstance instance, VkDebugUtilsMessengerCreateInfoEXT createInfo);
	VulkanAppDebugger() = default;
};