#pragma once

#include <vulkan/vulkan.hpp>

struct VulkanFrame {
	vk::Device device;
	vk::UniqueImageView image_view;

	vk::UniqueFramebuffer framebuffer;

	vk::UniqueCommandBuffer command_buffer;

	vk::UniqueFence fence;

	vk::UniqueDescriptorSet descriptor_set;
	VkBuffer uniform_buffer;
	VkDeviceMemory uniform_buffer_memory;

	void init_frame(vk::Device device);
	void create_framebuffer(vk::Extent2D swapchain_extent, vk::RenderPass &render_pass, vk::ImageView &depth_image_view);
	void create_command_buffers(vk::CommandPool command_pool);
	void create_sync_objects();
	void create_descriptor_set(vk::DescriptorPool descriptor_pool, vk::DescriptorSetLayout descriptor_set_layout, vk::Sampler texture_sampler, vk::ImageView texture_image_view);
	void create_uniform_buffer(VkPhysicalDevice physical_device);
	void delete_frame();
};
