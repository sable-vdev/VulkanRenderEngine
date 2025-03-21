#include "CommandBuffer.hpp"


void CommandBuffer::RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex)
{
	VkCommandBufferBeginInfo beginInfo = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.pNext = nullptr,
		.flags = 0,
		.pInheritanceInfo = nullptr
	};

	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to begin command buffer");
	}

	VkRect2D vkrect = {
		.offset = {0, 0},
		.extent = m_swapChainExtent
	};

	VkClearValue clearValue = { {{0.0f, 0.0f, 0.0f, 1.0f}} };

	VkRenderPassBeginInfo rpBeginInfo = {
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		.pNext = nullptr,
		.renderPass = m_renderPass,
		.framebuffer = m_framebuffer[imageIndex],
		.renderArea = vkrect,
		.clearValueCount = 1,
		.pClearValues = &clearValue
	};

	vkCmdBeginRenderPass(commandBuffer, &rpBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);

	VkViewport viewport = {
		.x = 0.0f,
		.y = 0.0f,
		.width = static_cast<float>(m_swapChainExtent.width),
		.height = static_cast<float>(m_swapChainExtent.height),
		.minDepth = 0.0f,
		.maxDepth = 1.0f
	};

	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

	VkRect2D scissor = {
		.offset = {0, 0},
		.extent = m_swapChainExtent
	};
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	vkCmdDraw(commandBuffer, 3, 1, 0, 0);

	vkCmdEndRenderPass(commandBuffer);

	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to end recording the command buffer");
	}
}

void CommandBuffer::CreateCommandPool(VkDevice& device, VkPhysicalDevice& physicalDecvice)
{
	QueueFamilies family;
	family.FindQueueFamilies(physicalDecvice);

	VkCommandPoolCreateInfo createInfo = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.pNext = nullptr,
		.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
		.queueFamilyIndex = family.graphicsFamily.has_value()
	};

	if (vkCreateCommandPool(device, &createInfo, nullptr, &m_commandPool) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create command pool");
	}
}

void CommandBuffer::CreateCommandBuffer(VkDevice& device, VkPhysicalDevice& physicalDecvice, VkExtent2D extent,
	std::vector<VkFramebuffer>& framebuffer, VkRenderPass renderPass, VkPipeline pipeline)
{
	CreateCommandPool(device, physicalDecvice);

	VkCommandBufferAllocateInfo allocateInfo = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.pNext = nullptr,
		.commandPool = m_commandPool,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = 1
	};

	if (vkAllocateCommandBuffers(device, &allocateInfo, &m_commandBuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate command buffers");
	}

	m_swapChainExtent = extent;
	m_framebuffer = framebuffer;
	m_renderPass = renderPass;
	m_pipeline = pipeline;
}

void CommandBuffer::DestroyCommandBuffer(VkDevice& device)
{
	vkDestroyCommandPool(device, m_commandPool, nullptr);
}
