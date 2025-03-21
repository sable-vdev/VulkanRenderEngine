#pragma once
#include "QueueFamilies.hpp"
#include <stdexcept>
#include <vector>
#include <limits>
#include <algorithm>
#include <iostream>

struct SwapChainSupportDetails
{
	VkSurfaceCapabilitiesKHR capabilities = {0};
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentMode;
};

class SwapChain
{
public:
	VkSwapchainKHR swapChain = nullptr;
	std::vector<VkImage> swapChainImages;
	VkFormat swapChainImageFormat{};
	VkExtent2D swapChainExtent{};

public:
	void CreateSwapChain(GLFWwindow* window, VkPhysicalDevice vkpd, VkDevice& device, VkSurfaceKHR& surface);
	void DestroySwapChain(VkDevice device);
	SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);
private:
	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& presentModes);
	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window);
};