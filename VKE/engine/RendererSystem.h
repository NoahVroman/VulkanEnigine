#pragma once

#include "Pipeline.h"
#include "Device.h"
#include "GameObject.h"
#include "Camera.h"
#include <numeric>

#include "FrameInfo.h"
#include <memory>
#include <vector>

namespace Engine
{
	class RendererSystem
	{
	public:
		RendererSystem(Device& device, VkRenderPass renderpass,VkDescriptorSetLayout globalSetLayout);
		~RendererSystem();

		RendererSystem(const RendererSystem&) = delete;
		RendererSystem& operator=(const RendererSystem&) = delete;

		void RenderGameObjects(FrameInfo& frameinfo, std::vector<GameObject>& gameobj);

	private:

		void CreatePipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void CreatePipeline(VkRenderPass renderpass);

		Device& m_Device;

		std::unique_ptr<Pipeline> m_Pipeline;
		std::unique_ptr<Pipeline> m_Pipeline2D;
		VkPipelineLayout m_PipelineLayout;


	};

};
