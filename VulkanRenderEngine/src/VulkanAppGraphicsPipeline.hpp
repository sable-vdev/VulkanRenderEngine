#pragma once
#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include <vector>
#include <string>
#include <fstream>

class VulkanAppGraphicsPipeline
{
private:
	VkRenderPass m_renderPass = nullptr;
	VkPipelineLayout m_pipelineLayout = nullptr;
	VkPipeline m_pipeline = nullptr;
public:
	void DestroyGraphicsPipeline(VkDevice& device);
	VulkanAppGraphicsPipeline() = default;
	VulkanAppGraphicsPipeline(VkDevice& device, VkFormat swapChainImageFormat);
private:
	void CreateGraphicsPipeline(VkDevice& device);
	void CreateRenderPass(VkDevice& device, VkFormat swapChainImageFormat);
	VkShaderModule CreateShaderModule(VkDevice& device, const std::vector<char>& code);
	inline std::vector<char> ReadFile(const std::string& filename)
	{
		std::ifstream file(filename, std::ios::ate | std::ios::binary);

		if (!file.is_open())
		{
			throw std::runtime_error("Failed to open file");
		}

		const size_t fileSize = file.tellg();
		std::vector<char> buffer(fileSize);
		file.seekg(0);
		file.read(buffer.data(), fileSize);
		file.close();

		return buffer;
	}
};

