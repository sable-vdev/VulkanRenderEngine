#include "SableRender.hpp"

const std::vector<const char*> validationLayers = {"VK_LAYER_KHRONOS_validation"};

#ifdef NDEBUG
const bool ENABLE_VALIDATION_LAYERS = false;
#else
const bool ENABLE_VALIDATION_LAYERS = true;
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
void SableRender::Run()
{
	InitGlfw();
	InitVulkan();
	MainLoop();
	CleanUp();
}
/*
* Initializing GLFW and creating the window
*/
void SableRender::InitGlfw()
{
	glfwInit();
	
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	m_window = glfwCreateWindow(WIDTH, HEIGHT, TITLE, nullptr, nullptr);
}
/*
* Initializing the Vulkan Instance
*/
void SableRender::InitVulkan()
{
	CreateVulkanInstance();
	SetupDebugMessenger();
	CreateSurfaceGlfw();
	CreatePhysicalDevice();
	CreateLogicalDevice();
	CreateSwapChain();
	CreateImageViews();
	CreateGraphicsPipeline();
	CreateFramebuffer();
	CreateCommandBuffer();
	CreateSyncObjects();
}

/*
* Main loop of the application for rendering and window
*/
void SableRender::MainLoop()
{
	m_frameCount = 0.0f;
	m_lastDeltaTime = glfwGetTime();
	while (!glfwWindowShouldClose(m_window))
	{
		CalculateFps();
		glfwPollEvents();
		DrawFrame();
	}

	vkDeviceWaitIdle(m_sableLogicalDevice.vulkanDevice);
}
void SableRender::DrawFrame()
{
	vkWaitForFences(m_sableLogicalDevice.vulkanDevice, 1, &m_inFlightFence, VK_TRUE, UINT64_MAX);
	vkResetFences(m_sableLogicalDevice.vulkanDevice, 1, &m_inFlightFence);

	VkFenceCreateInfo fenceInfo = {
		.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		.pNext = nullptr,
		.flags = VK_FENCE_CREATE_SIGNALED_BIT
	};
	//aquire image from swap chain
	uint32_t imageIndex;
	vkAcquireNextImageKHR(m_sableLogicalDevice.vulkanDevice, m_sableSwapChain.swapChain, UINT32_MAX, m_imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);
	vkResetCommandBuffer(m_sableCommandBuffer.commandBuffer, 0);
	m_sableCommandBuffer.RecordCommandBuffer(imageIndex);

	VkSemaphore waitSemaphores[] = { m_imageAvailableSemaphore };
	VkSemaphore signalSemaphores[] = { m_renderFinishedSemaphore };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

	VkSubmitInfo submitInfo = {
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.pNext = nullptr,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = waitSemaphores,
		.pWaitDstStageMask = waitStages,
		.commandBufferCount = 1,
		.pCommandBuffers = &m_sableCommandBuffer.commandBuffer,
		.signalSemaphoreCount = 1,
		.pSignalSemaphores = signalSemaphores
	};

	if (vkQueueSubmit(m_sableLogicalDevice.graphicsQueue, 1, &submitInfo, m_inFlightFence) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to submit draw command buffer");
	}

	VkPresentInfoKHR presentInfo = {  
		.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		.pNext = nullptr,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = signalSemaphores,
		.swapchainCount = 1,
		.pSwapchains = &m_sableSwapChain.swapChain,
		.pImageIndices = &imageIndex,
		.pResults = nullptr
	};

	vkQueuePresentKHR(m_sableLogicalDevice.presentQueue, &presentInfo);
}

void SableRender::CalculateFps()
{
	float deltaTime = glfwGetTime();
	if (deltaTime - m_lastDeltaTime >= 1.0)
	{
		std::cout << m_frameCount << " (" << 1000.0f / m_frameCount << " ms)\n";
		m_frameCount = 0;
		m_lastDeltaTime = deltaTime;
	}
	m_frameCount++;
}
/*
* Destroy every resource and making up the space after the app has been closed
*/
void SableRender::CleanUp()
{
	vkDestroySemaphore(m_sableLogicalDevice.vulkanDevice, m_imageAvailableSemaphore, nullptr);
	vkDestroySemaphore(m_sableLogicalDevice.vulkanDevice, m_renderFinishedSemaphore, nullptr);
	vkDestroyFence(m_sableLogicalDevice.vulkanDevice, m_inFlightFence, nullptr);

	m_sableCommandBuffer.DestroyCommandBuffer(m_sableLogicalDevice.vulkanDevice);
	m_sableGraphicsPipeline.DestroyGraphicsPipeline(m_sableLogicalDevice.vulkanDevice);
	m_sableImageView.DestroyImageViews(m_sableLogicalDevice.vulkanDevice);
	m_sableSwapChain.DestroySwapChain(m_sableLogicalDevice.vulkanDevice);
	m_sableFramebuffer.DestroyFramebuffers(m_sableLogicalDevice.vulkanDevice);

	m_sableLogicalDevice.DestroyLogicalDevice();

	if (ENABLE_VALIDATION_LAYERS)
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
void SableRender::CreateVulkanInstance()
{
	if (ENABLE_VALIDATION_LAYERS && !CheckValidationLayerSupport())
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

	if constexpr(ENABLE_VALIDATION_LAYERS)
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

void SableRender::CreateLogicalDevice()
{
	QueueFamilies family;
	family = family.FindQueueFamilies(m_sablePhysicalDevice.vulkanAppPhysicalDevice, m_surfaceKhr);
	m_sableLogicalDevice = LogicalDevice(family, m_sablePhysicalDevice.vulkanAppPhysicalDevice, ENABLE_VALIDATION_LAYERS, deviceExtensions, validationLayers);
}

std::vector<const char*> SableRender::GetRequiredExtensions()
{
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if constexpr(ENABLE_VALIDATION_LAYERS)
	{
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;
}

void SableRender::SetupDebugMessenger()
{
	if constexpr(!ENABLE_VALIDATION_LAYERS) return;
	VkDebugUtilsMessengerCreateInfoEXT createInfo{};
	m_debugger = Debugger(m_instance, createInfo);
}

void SableRender::CreateSurfaceGlfw()
{
	if (glfwCreateWindowSurface(m_instance, m_window, nullptr, &m_surfaceKhr) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create window surface");
	}
}

void SableRender::CreatePhysicalDevice()
{
	m_sablePhysicalDevice.GetPhysicalDevice(m_instance, m_surfaceKhr);
}

void SableRender::CreateSwapChain()
{
	m_sableSwapChain.CreateSwapChain(m_window, m_sablePhysicalDevice.vulkanAppPhysicalDevice, m_sableLogicalDevice.vulkanDevice, m_surfaceKhr);
}

void SableRender::CreateImageViews()
{
	m_sableImageView.CreateImageViews(m_sableLogicalDevice.vulkanDevice, m_sableSwapChain);
}

void SableRender::CreateGraphicsPipeline()
{
	m_sableGraphicsPipeline.CreateGraphicsPipeline(m_sableLogicalDevice.vulkanDevice, m_sableSwapChain.swapChainImageFormat);
}

void SableRender::CreateFramebuffer()
{
	m_sableFramebuffer.CreateFramebuffers(m_sableLogicalDevice.vulkanDevice, m_sableImageView.imageViews, m_sableSwapChain.swapChainExtent, m_sableGraphicsPipeline.renderPass);
}
void SableRender::CreateCommandBuffer()
{
	m_sableCommandBuffer.CreateCommandBuffer(m_sableLogicalDevice.vulkanDevice, m_sablePhysicalDevice.vulkanAppPhysicalDevice,
		m_sableSwapChain.swapChainExtent, m_sableFramebuffer.frameBuffers, m_sableGraphicsPipeline.renderPass, m_sableGraphicsPipeline.pipeline);
}

void SableRender::CreateSyncObjects()
{
	VkSemaphoreCreateInfo semaphoreCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0
	};

	VkFenceCreateInfo fenceInfo = {
		.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0
	};

	if (vkCreateSemaphore(m_sableLogicalDevice.vulkanDevice, &semaphoreCreateInfo, nullptr, &m_imageAvailableSemaphore) != VK_SUCCESS ||
		vkCreateSemaphore(m_sableLogicalDevice.vulkanDevice, &semaphoreCreateInfo, nullptr, &m_renderFinishedSemaphore) != VK_SUCCESS ||
		vkCreateFence(m_sableLogicalDevice.vulkanDevice, &fenceInfo, nullptr, &m_inFlightFence) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create semaphores!");
	}
}
