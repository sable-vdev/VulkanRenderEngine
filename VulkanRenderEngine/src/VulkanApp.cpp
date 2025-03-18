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
	InitGlfw();
	InitVulkan();
	MainLoop();
	CleanUp();
}
/*
* Initializing GLFW and creating the window
*/
void VulkanApp::InitGlfw()
{
	glfwInit();
	
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	m_window = glfwCreateWindow(WIDTH, HEIGHT, TITLE, nullptr, nullptr);
}
/*
* Initializing the Vulkan Instance
*/
void VulkanApp::InitVulkan()
{
	CreateVulkanInstance();
	SetupDebugMessenger();
	CreateSurfaceGlfw();
	m_vulkanAppPhysicalDevice.GetPhysicalDevice(m_instance, m_surfaceKhr);
	CreateLogicalDevice();
	CreateSwapChain();
	m_vulkanAppImageView.CreateImageViews(m_vulkanAppLogicalDevice.vulkanDevice, m_vulkanAppSwapChain);
	m_vulkanAppGraphicsPipeline = VulkanAppGraphicsPipeline(m_vulkanAppLogicalDevice.vulkanDevice, m_vulkanAppSwapChain.swapChainImageFormat);
	CreateFramebuffer();
}

/*
* Main loop of the application for rendering and window
*/
void VulkanApp::MainLoop() const
{
	float frameCount = 0.0f;
	double lastDeltaTime = glfwGetTime();


	while (!glfwWindowShouldClose(m_window))
	{
		double deltaTime = glfwGetTime();
		if (deltaTime - lastDeltaTime >= 1.0)
		{
			std::cout << frameCount << " (" << 1000.0f / frameCount << " ms)\n";
			frameCount = 0;
			lastDeltaTime = deltaTime;
		}
		frameCount++;

		glfwPollEvents();
	}
}
/*
* Destroy every resource and making up the space after the app has been closed
*/
void VulkanApp::CleanUp()
{
	m_vulkanAppGraphicsPipeline.DestroyGraphicsPipeline(m_vulkanAppLogicalDevice.vulkanDevice);
	m_vulkanAppImageView.DestroyImageViews(m_vulkanAppLogicalDevice.vulkanDevice);
	m_vulkanAppSwapChain.DestroySwapChain(m_vulkanAppLogicalDevice.vulkanDevice);
	m_vulkanAppFramebuffer.DestroyFramebuffers(m_vulkanAppLogicalDevice.vulkanDevice);

	m_vulkanAppLogicalDevice.DestroyLogicalDevice();

	if (enableValidationLayers)
	{
		m_debugger.DestroyDebugUtilsMessengerEXT();
	}

	vkDestroySurfaceKHR(m_instance, m_surfaceKhr, nullptr);

	vkDestroyInstance(m_instance, nullptr);

	glfwDestroyWindow(m_window);

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
	constexpr VkApplicationInfo appInfo
	{ 
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pNext = nullptr,
		.pApplicationName = "Vulkan Engine",
		.applicationVersion = VK_MAKE_API_VERSION(0, 0, 1, 0),
		.pEngineName = "Vulkan Engine",
		.engineVersion = VK_MAKE_API_VERSION(0, 0, 1, 0),
		.apiVersion = VK_API_VERSION_1_0
	}; 

	/*
	* mandatory information of the Vulkan application
	*/
	VkInstanceCreateInfo createInfo{};

	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	
	const auto extensions = GetRequiredExtensions();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};

	if constexpr(enableValidationLayers)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();

		m_debugger.PopulateDebugMessengerCreateInfo(debugCreateInfo);
		createInfo.pNext = &debugCreateInfo;
	}
	else 
	{ 
		createInfo.enabledLayerCount = 0;
		createInfo.pNext = nullptr;
	}

	//Creating the instance
	if (vkCreateInstance(&createInfo, nullptr, &m_instance) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create the Vulkan instance");
	}	
}

void VulkanApp::CreateLogicalDevice()
{
	VulkanAppQueueFamilies family;
	family = family.FindQueueFamilies(m_vulkanAppPhysicalDevice.vulkanAppPhysicalDevice, m_surfaceKhr);
	m_vulkanAppLogicalDevice = VulkanAppLogicalDevice(family, m_vulkanAppPhysicalDevice.vulkanAppPhysicalDevice, enableValidationLayers, deviceExtensions, validationLayers);
}

std::vector<const char*> VulkanApp::GetRequiredExtensions()
{
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if constexpr(enableValidationLayers)
	{
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;
}

void VulkanApp::SetupDebugMessenger()
{
	if constexpr(!enableValidationLayers) return;
	VkDebugUtilsMessengerCreateInfoEXT createInfo{};
	m_debugger = VulkanAppDebugger(m_instance, createInfo);
}

void VulkanApp::CreateSurfaceGlfw()
{
	if (glfwCreateWindowSurface(m_instance, m_window, nullptr, &m_surfaceKhr) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create window surface");
	}
}

void VulkanApp::CreateSwapChain()
{
	m_vulkanAppSwapChain.CreateSwapChain(m_window, m_vulkanAppPhysicalDevice.vulkanAppPhysicalDevice, m_vulkanAppLogicalDevice.vulkanDevice, m_surfaceKhr);
}

void VulkanApp::CreateFramebuffer()
{
	m_vulkanAppFramebuffer.CreateFramebuffers(m_vulkanAppLogicalDevice.vulkanDevice, m_vulkanAppImageView.imageViews, m_vulkanAppSwapChain.swapChainExtent, m_vulkanAppGraphicsPipeline.renderPass);
}
