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

class Debugger
{
private:
	VkDebugUtilsMessengerEXT mDebugMessenger = nullptr;
	VkInstance mInstance = nullptr;
private:
	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
	VkResult CreateDebugUtilsMessengerExt(const VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
public:
	void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	void DestroyDebugUtilsMessengerEXT(const VkAllocationCallbacks* pAllocator = nullptr);
	Debugger(const VkInstance instance, VkDebugUtilsMessengerCreateInfoEXT createInfo);
	Debugger() = default;
};