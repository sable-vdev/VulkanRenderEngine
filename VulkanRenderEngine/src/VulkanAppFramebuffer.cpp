#include "VulkanAppFramebuffer.hpp"

void VulkanAppFramebuffer::CreateFramebuffers(VkDevice& device, std::vector<VkImageView>& imageViews, VkExtent2D swapChainExtent, VkRenderPass& renderPass)
{
	m_frameBuffers.reserve(imageViews.size());

	for (auto i = 0; i < imageViews.size(); i++)
	{
		VkImageView attachments[] = {
			imageViews[i]
		};

		VkFramebufferCreateInfo framebufferInfo = {
			.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.renderPass = renderPass,
			.attachmentCount = 1,
			.pAttachments = attachments,
			.width = swapChainExtent.width,
			.height = swapChainExtent.height,
			.layers = 1
		};

		VkFramebuffer framebuffer;

		if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &m_frameBuffers.emplace_back(framebuffer)) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create framebuffer");
		}
	}
}

void VulkanAppFramebuffer::DestroyFramebuffers(VkDevice& device)
{
	for (const auto buffer : m_frameBuffers)
	{
		vkDestroyFramebuffer(device, buffer, nullptr);
	}
}
