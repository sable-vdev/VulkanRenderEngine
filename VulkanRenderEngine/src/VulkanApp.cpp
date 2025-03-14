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

	m_Window = glfwCreateWindow(WIDTH, HEIGHT, TITLE, nullptr, nullptr);
}
/*
* Initializing the Vulkan Instance
*/
void VulkanApp::InitVulkan()
{
	CreateVulkanInstance();
	SetupDebugMessenger();
	CreateSurfaceGlfw();
	m_vulkanAppPhysicalDevice.GetPhysicalDevice(m_Instance, m_surfaceKhr);
	CreateLogicalDevice();
	CreateSwapChain();
	m_vulkanAppImageView.CreateImageViews(m_vulkanAppLogicalDevice.vulkanDevice, m_vulkanAppSwapChain);
	CreateGraphicsPipeline();
}
/*
* Main loop of the application for rendering and window
*/
void VulkanApp::MainLoop()
{
	float frameCount = 0.0f;
	double lastDeltaTime = glfwGetTime();


	while (!glfwWindowShouldClose(m_Window))
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
	vkDestroyPipelineLayout(m_vulkanAppLogicalDevice.vulkanDevice, m_pipelineLayout, nullptr);

	m_vulkanAppImageView.DestroyImageViews(m_vulkanAppLogicalDevice.vulkanDevice);
	//vkDestroySwapchainKHR(mVulkanAppLogicalDevice.vulkanDevice, mSwapChain, nullptr);
	m_vulkanAppSwapChain.DestroySwapChain(m_vulkanAppLogicalDevice.vulkanDevice, nullptr);
	//vkDestroyDevice(mVulkanAppLogicalDevice.vulkanDevice, nullptr);
	m_vulkanAppLogicalDevice.DestroyLogicalDevice();

	if (enableValidationLayers)
	{
		m_debugger.DestroyDebugUtilsMessengerEXT();
	}

	vkDestroySurfaceKHR(m_Instance, m_surfaceKhr, nullptr);

	vkDestroyInstance(m_Instance, nullptr);

	glfwDestroyWindow(m_Window);

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
	
	const auto extensions = GetRequiredExtensions();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};

	if (enableValidationLayers)
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
	if (vkCreateInstance(&createInfo, nullptr, &m_Instance) != VK_SUCCESS)
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

	if (enableValidationLayers)
	{
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;
}

void VulkanApp::SetupDebugMessenger()
{
	if constexpr(!enableValidationLayers) return;
	constexpr VkDebugUtilsMessengerCreateInfoEXT createInfo{};
	m_debugger = VulkanAppDebugger(m_Instance, createInfo);
}

void VulkanApp::CreateSurfaceGlfw()
{
	if (glfwCreateWindowSurface(m_Instance, m_Window, nullptr, &m_surfaceKhr) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create window surface");
	}
}

void VulkanApp::CreateSwapChain()
{
	m_vulkanAppSwapChain.CreateSwapChain(m_Window, m_vulkanAppPhysicalDevice.vulkanAppPhysicalDevice, m_vulkanAppLogicalDevice.vulkanDevice, m_surfaceKhr);
}

/*
* TODO: CONTAINERCLASS FOR GRAPHICSPIPELINE
*/

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

	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {
		VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		nullptr,
		0,
		0,
		nullptr,
		0,
		nullptr
	};

	VkPipelineInputAssemblyStateCreateInfo assemblyInputInfo = {
		VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
		nullptr,
		0,
		VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
		VK_FALSE
	};

	VkPipelineViewportStateCreateInfo viewportCreateInfo = {
		VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
		nullptr,
		0,
		1,
		nullptr,
		1,
		nullptr
	};

	VkPipelineRasterizationStateCreateInfo rasterizationCreateInfo = {
		VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
		nullptr,
		0,
		VK_FALSE,
		VK_FALSE,
		VK_POLYGON_MODE_FILL,
		VK_CULL_MODE_BACK_BIT,
		VK_FRONT_FACE_CLOCKWISE,
		VK_FALSE,
		0.0f,
		0.0f,
		0.0f,
		1.0f
	};

	VkPipelineMultisampleStateCreateInfo multisampleCreateInfo = {
		VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
		nullptr,
		0,
		VK_SAMPLE_COUNT_1_BIT,
		VK_FALSE,
		1.0f,
		nullptr,
		VK_FALSE,
		VK_FALSE
	};

	VkPipelineColorBlendAttachmentState colorBlendAttachment = {
		VK_FALSE,
		VK_BLEND_FACTOR_ONE,
		VK_BLEND_FACTOR_ZERO,
		VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_ONE,
		VK_BLEND_FACTOR_ZERO,
		VK_BLEND_OP_ADD,
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_A_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_G_BIT
	};

	VkPipelineColorBlendStateCreateInfo colorBlendStateInfo = {
		VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
		nullptr,
		0,
		VK_FALSE,
		VK_LOGIC_OP_COPY,
		1,
		&colorBlendAttachment,
		{0.0f, 0.0f, 0.0f, 0.0f}
	};

	std::vector dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

	VkPipelineDynamicStateCreateInfo dynamicState = {
		VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
		nullptr,
		0,
		static_cast<uint32_t>(dynamicStates.size()),
		dynamicStates.data()
	};

	VkPipelineLayoutCreateInfo pipelineCreateInfo = {
		VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		nullptr,
		0,
		0,
		nullptr,
		0,
		nullptr,
	};

	if (vkCreatePipelineLayout(m_vulkanAppLogicalDevice.vulkanDevice, &pipelineCreateInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create pipeline layout");
	}

	vkDestroyShaderModule(m_vulkanAppLogicalDevice.vulkanDevice, vertexModule, nullptr);
	vkDestroyShaderModule(m_vulkanAppLogicalDevice.vulkanDevice, fragmentModule, nullptr);
}

VkShaderModule VulkanApp::CreateShaderModule(const std::vector<char>& code)
{
	const VkShaderModuleCreateInfo createInfo{
		VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		nullptr,
		0,
		code.size(),
		reinterpret_cast<const uint32_t*>(code.data())
	};
	VkShaderModule shaderModule;

	if (vkCreateShaderModule(m_vulkanAppLogicalDevice.vulkanDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create shader module");
	}

	return shaderModule;
}
