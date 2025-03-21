#pragma once
#include "QueueFamilies.hpp"
#include <stdexcept>

class CommandBuffer
{
private:
	VkCommandPool m_commandPool;
	VkCommandBuffer m_commandBuffer;
	VkExtent2D m_swapChainExtent;
	std::vector<VkFramebuffer> m_framebuffer;
	VkRenderPass m_renderPass;
	VkPipeline m_pipeline;
private:
	void CreateCommandPool(VkDevice& device, VkPhysicalDevice& physicalDecvice);
public:
	void RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
	void CreateCommandBuffer(VkDevice& device, VkPhysicalDevice& physicalDecvice, VkExtent2D extent, std::vector<VkFramebuffer>& framebuffer, VkRenderPass renderPass, VkPipeline pipeline);
	void DestroyCommandBuffer(VkDevice& device);
};

