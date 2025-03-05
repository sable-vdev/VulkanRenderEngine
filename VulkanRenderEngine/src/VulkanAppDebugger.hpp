#pragma once
#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include <iostream>

enum LogLevel
{
	INFO = 0,
	VERBOSE = 1,
	WARNING = 2,
	ERROR = 3
};

class VulkanAppDebugger
{
private:
	VkDebugUtilsMessengerEXT mDebugMessenger = nullptr;
	VkInstance mInstance = nullptr;
private:
	//void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
	VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
public:
	void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
public:
	void DestroyDebugUtilsMessengerEXT(const VkAllocationCallbacks* pAllocator = nullptr);
	VulkanAppDebugger(VkInstance instance, VkDebugUtilsMessengerCreateInfoEXT createInfo);
	VulkanAppDebugger();
};