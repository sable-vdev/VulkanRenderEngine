#include "VulkanApp.hpp"

const std::vector<const char*> validationLayers = {"VK_LAYER_KHRONOS_validation"};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

bool CheckValidationLayerSupport()
{
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layerName : validationLayers)
	{
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers)
		{
			if (strcmp(layerName, layerProperties.layerName) == 0)
			{
				layerFound = true;
				break;
			}
		}
		if (!layerFound) return false;
	}

	return true;
}

/*
* Vulkanapp init function handling the app
*/
void VulkanApp::Run()
{
	InitGLFW();
	InitVulkan();
	MainLoop();
	CleanUp();
}
/*
* Initializing GLFW and creating the window
*/
void VulkanApp::InitGLFW()
{
	glfwInit();
	
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	mWindow = glfwCreateWindow(WIDTH, HEIGHT, "Hello Vulkan", nullptr, nullptr);
}
/*
* Initializing the Vulkan Instance
*/
void VulkanApp::InitVulkan()
{
	CreateVulkanInstance();
	SetupDebugMessenger();
	CreateSurfaceGLFW();
	//GetPhysicalDevices();
	mVulkanAppPhysicalDevice.GetPhysicalDevice(mInstance, mSurfaceKHR);
	CreateLogicalDevice();
	CreateSwapChain();
	mVulkanAppImageView.CreateImageViews(mVulkanAppLogicalDevice.vulkanDevice, mVulkanAppSwapChain);
}
/*
* Main loop of the application for rendering and window
*/
void VulkanApp::MainLoop()
{
	double deltaTime, lastDeltaTime;
	int framecount = 0;
	lastDeltaTime = glfwGetTime();


	while (!glfwWindowShouldClose(mWindow))
	{
		deltaTime = glfwGetTime();
		if (deltaTime - lastDeltaTime >= 1.0)
		{
			std::cout << framecount << '\n';
			framecount = 0;
			lastDeltaTime = deltaTime;
		}
		framecount++;

		glfwPollEvents();
	}
}
/*
* Destroy every resource and making up the space after the app has been closed
*/
void VulkanApp::CleanUp()
{
	mVulkanAppImageView.DestroyImageViews(mVulkanAppLogicalDevice.vulkanDevice);
	//vkDestroySwapchainKHR(mVulkanAppLogicalDevice.vulkanDevice, mSwapChain, nullptr);
	mVulkanAppSwapChain.DestroySwapChain(mVulkanAppLogicalDevice.vulkanDevice, nullptr);
	//vkDestroyDevice(mVulkanAppLogicalDevice.vulkanDevice, nullptr);
	mVulkanAppLogicalDevice.DestoryLogicalDevice();

	if (enableValidationLayers)
	{
		mDebugger.DestroyDebugUtilsMessengerEXT();
	}

	vkDestroySurfaceKHR(mInstance, mSurfaceKHR, nullptr);

	vkDestroyInstance(mInstance, nullptr);

	glfwDestroyWindow(mWindow);

	glfwTerminate();
}
/*
* Creating the Vulkan Instance
*/
void VulkanApp::CreateVulkanInstance()
{
	if (enableValidationLayers && !CheckValidationLayerSupport())
	{
		throw std::runtime_error("Validation layers requested, but not available!");
	}

	/*
	* General optional information about the vulkan application
	*/
	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Vulkan";
	appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 0, 1, 0);
	appInfo.pEngineName = "Vulkan";
	appInfo.engineVersion = VK_MAKE_API_VERSION(0, 0, 1, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	/*
	* mandatory information of the Vulkan application
	*/
	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	
	auto extensions = GetRequiredExtensions();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};

	if (enableValidationLayers)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();

		mDebugger.PopulateDebugMessengerCreateInfo(debugCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
	}
	else 
	{ 
		createInfo.enabledLayerCount = 0;
		createInfo.pNext = nullptr;
	}

	//Creating the instance
	if (vkCreateInstance(&createInfo, nullptr, &mInstance) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create the Vulkan instance");
	}	
}
/*
void VulkanApp::GetPhysicalDevices()
{
	/*
	VulkanAppPhysicalDevice vkapd;

	mPhysicalDevice = vkapd.GetPhysicalDevice(mInstance, mSurfaceKHR);
	
	mVulkanAppPhysicalDevice.GetPhysicalDevice(mInstance, mSurfaceKHR);

	glfwShowWindow(mWindow);
}
*/
void VulkanApp::CreateLogicalDevice()
{
	//VulkanAppQueueFamilies family = family.FindQueueFamilies(mPhysicalDevice, mSurfaceKHR);
	VulkanAppQueueFamilies family = family.FindQueueFamilies(mVulkanAppPhysicalDevice.vulkanAppPhysicalDevice, mSurfaceKHR);
	/*
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos{};
	std::set<uint32_t> uniqueQueueFamilies = { family.graphicsFamily.value(), family.presentFamily.value() };
	
	float queuePriority = 1.0f;

	for (uint32_t queueFamily : uniqueQueueFamilies)
	{
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures{};

	VkDeviceCreateInfo deviceCreateInfo{};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
	deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
	deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();

	if (enableValidationLayers)
	{
		deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		deviceCreateInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else deviceCreateInfo.enabledLayerCount = 0;
	/*
	if (vkCreateDevice(mPhysicalDevice, &deviceCreateInfo, nullptr, &mDevice) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create logical device");
	}
	//
	if (vkCreateDevice(mVulkanAppPhysicalDevice.vulkanAppPhysicalDevice, &deviceCreateInfo, nullptr, &mDevice) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create logical device");
	}

	vkGetDeviceQueue(mDevice, family.graphicsFamily.value(), 0, &mGraphicsQueue);
	vkGetDeviceQueue(mDevice, family.presentFamily.value(), 0, &mPresentQueue);
	*/

	mVulkanAppLogicalDevice = VulkanAppLogicalDevice(family, mVulkanAppPhysicalDevice.vulkanAppPhysicalDevice, enableValidationLayers, deviceExtensions, validationLayers);
}

std::vector<const char*> VulkanApp::GetRequiredExtensions()
{
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (enableValidationLayers)
	{
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;
}
/*
void VulkanApp::SetupDebugMessenger()
{
	if (!enableValidationLayers) return;

	VkDebugUtilsMessengerCreateInfoEXT createInfo;
	mDebugger = VulkanAppDebugger(mInstance, createInfo);

	//mDebugger.PopulateDebugMessengerCreateInfo(createInfo);

	if (mDebugger.CreateDebugUtilsMessengerEXT(mInstance, &createInfo, nullptr, &mDebugMessenger) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to setup debug messenger!");
	}
}
*/

void VulkanApp::SetupDebugMessenger()
{
	if (!enableValidationLayers) return;
	VkDebugUtilsMessengerCreateInfoEXT createInfo{};
	mDebugger = VulkanAppDebugger(mInstance, createInfo);
}

void VulkanApp::CreateSurfaceGLFW()
{
	if (glfwCreateWindowSurface(mInstance, mWindow, nullptr, &mSurfaceKHR) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create window surface");
	}
}

void VulkanApp::CreateSwapChain()
{
	/*
	//SwapChainSupportDetails swapChainDetails = QuerySwapChainSupport(mPhysicalDevice, mSurfaceKHR);
	SwapChainSupportDetails swapChainDetails = QuerySwapChainSupport(mVulkanAppPhysicalDevice.vulkanAppPhysicalDevice, mSurfaceKHR);

	VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainDetails.formats);
	VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainDetails.presentMode);
	VkExtent2D extent = ChooseSwapExtent(swapChainDetails.capabilities, mWindow);

	uint32_t imageCount = swapChainDetails.capabilities.minImageCount + 1;

	if (swapChainDetails.capabilities.maxImageCount > 0 && imageCount > swapChainDetails.capabilities.maxImageCount)
	{
		imageCount = swapChainDetails.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = mSurfaceKHR;
	createInfo.minImageCount = imageCount;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	//VulkanAppQueueFamilies families = families.FindQueueFamilies(mPhysicalDevice, mSurfaceKHR);
	VulkanAppQueueFamilies families = families.FindQueueFamilies(mVulkanAppPhysicalDevice.vulkanAppPhysicalDevice, mSurfaceKHR);
	uint32_t queueFamilyIndices[] = { families.graphicsFamily.value(), families.presentFamily.value() };

	if (families.graphicsFamily != families.presentFamily)
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices = nullptr;
	}

	createInfo.preTransform = swapChainDetails.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = nullptr;
	/*
	if (vkCreateSwapchainKHR(mDevice, &createInfo, nullptr, &mSwapChain) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create swap chain");
	}

	vkGetSwapchainImagesKHR(mDevice, mSwapChain, &imageCount, nullptr);
	mSwapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(mDevice, mSwapChain, &imageCount, mSwapChainImages.data());
	//

	if (vkCreateSwapchainKHR(mVulkanAppLogicalDevice.vulkanDevice, &createInfo, nullptr, &mSwapChain) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create swap chain");
	}

	vkGetSwapchainImagesKHR(mVulkanAppLogicalDevice.vulkanDevice, mSwapChain, &imageCount, nullptr);
	mSwapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(mVulkanAppLogicalDevice.vulkanDevice, mSwapChain, &imageCount, mSwapChainImages.data());

	mSwapChainImageFormat = surfaceFormat.format;
	mSwapChainExtent = extent;
	*/

	mVulkanAppSwapChain.CreateSwapChain(mWindow, mVulkanAppPhysicalDevice.vulkanAppPhysicalDevice, mVulkanAppLogicalDevice.vulkanDevice, mSurfaceKHR);
}