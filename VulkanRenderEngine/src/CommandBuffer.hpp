#pragma once
#include "QueueFamilies.hpp"
#include <stdexcept>

class CommandBuffer
{
private:
	VkCommandPool m_commandPool = nullptr;
	VkCommandBuffer m_commandBuffer = nullptr;
	VkExtent2D m_swapChainExtent = {};
	std::vector<VkFramebuffer> m_framebuffer = {};
	VkRenderPass m_renderPass = nullptr;
	VkPipeline m_pipeline = nullptr;
private:
	void CreateCommandPool(const VkDevice& device, const VkPhysicalDevice& physicalDecvice);
public:
	void RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
	void CreateCommandBuffer(const VkDevice& device, const VkPhysicalDevice& physicalDecvice, const VkExtent2D extent,
		const std::vector<VkFramebuffer>& framebuffer, const VkRenderPass renderPass, const VkPipeline pipeline);
	void DestroyCommandBuffer(const VkDevice& device) const;
};

