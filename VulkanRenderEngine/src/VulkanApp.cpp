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
	CreateGraphicsPipeline();
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
	VkApplicationInfo appInfo
	{ 
		VK_STRUCTURE_TYPE_APPLICATION_INFO,
		nullptr,
		"Vulkan Engine",
		VK_MAKE_API_VERSION(0, 0, 1, 0),
		"Vulkan Engine",
		VK_MAKE_API_VERSION(0, 0, 1, 0),
		VK_API_VERSION_1_0
	}; 

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

void VulkanApp::CreateLogicalDevice()
{
	VulkanAppQueueFamilies family = family.FindQueueFamilies(mVulkanAppPhysicalDevice.vulkanAppPhysicalDevice, mSurfaceKHR);
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
	mVulkanAppSwapChain.CreateSwapChain(mWindow, mVulkanAppPhysicalDevice.vulkanAppPhysicalDevice, mVulkanAppLogicalDevice.vulkanDevice, mSurfaceKHR);
}

void VulkanApp::CreateGraphicsPipeline()
{
	auto vertShaderCode = ReadFile("shadersrc/vert.spv");
	auto fragShaderCode = ReadFile("shadersrc/frag.spv");

	VkShaderModule vertexModule = CreateShaderModule(vertShaderCode);
	VkShaderModule fragmentModule = CreateShaderModule(fragShaderCode);

	VkPipelineShaderStageCreateInfo vertShaderCreateInfo{
		VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		nullptr,
		0,
		VK_SHADER_STAGE_VERTEX_BIT,
		vertexModule,
		"main",
		nullptr
	};

	VkPipelineShaderStageCreateInfo fragShaderCreateInfo{
		VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		nullptr,
		0,
		VK_SHADER_STAGE_FRAGMENT_BIT,
		fragmentModule,
		"main",
		nullptr
	};

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderCreateInfo, fragShaderCreateInfo };

	vkDestroyShaderModule(mVulkanAppLogicalDevice.vulkanDevice, vertexModule, nullptr);
	vkDestroyShaderModule(mVulkanAppLogicalDevice.vulkanDevice, fragmentModule, nullptr);
}

VkShaderModule VulkanApp::CreateShaderModule(const std::vector<char>& code)
{
	VkShaderModuleCreateInfo createInfo{
		VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		nullptr,
		0,
		code.size(),
		reinterpret_cast<const uint32_t*>(code.data())
	};
	VkShaderModule shaderModule;

	if (vkCreateShaderModule(mVulkanAppLogicalDevice.vulkanDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create shader module");
	}

	return shaderModule;
}
