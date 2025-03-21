#include "Debugger.hpp"

void Debugger::DestroyDebugUtilsMessengerEXT(const VkAllocationCallbacks* pAllocator)
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(mInstance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr)
	{
		func(mInstance, mDebugMessenger, pAllocator);
	}
}

VKAPI_ATTR VkBool32 VKAPI_CALL Debugger::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
	constexpr LogLevel log = WARNING;
	switch (messageSeverity)
	{
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
		if constexpr (log == INFO)
			std::cerr << "[INFO]  " << '\t' << pCallbackData->pMessage << '\n';
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
		if constexpr(log <= VERBOSE)
			std::cerr << "[VERBOSE]" << '\t' << pCallbackData->pMessage << '\n';
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
		if constexpr(log <= WARNING)
			std::cerr << "[WARNING]" << '\t' << pCallbackData->pMessage << '\n';
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
		if constexpr(log <= ERROR)
			std::cerr << "[ERROR] " << '\t' << pCallbackData->pMessage << '\n';
		break;
	default:
		break;
	}
	return VK_FALSE;
}

VkResult Debugger::CreateDebugUtilsMessengerExt(const VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
	const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
{
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr)
	{
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	else return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void Debugger::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
	createInfo = {
		.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
		.pNext = nullptr,
		.flags = 0,
		.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
		.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
		.pfnUserCallback = DebugCallback,
		.pUserData = nullptr
	};
}

Debugger::Debugger(const VkInstance instance, VkDebugUtilsMessengerCreateInfoEXT createInfo)
{
	mInstance = instance;
	PopulateDebugMessengerCreateInfo(createInfo);

	if(CreateDebugUtilsMessengerExt(mInstance, &createInfo, nullptr, &mDebugMessenger) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to setup debug messenger!");
	}
}