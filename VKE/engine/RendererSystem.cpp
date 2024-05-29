#include "RendererSystem.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>




#include <stdexcept>
#include <array>
#include <iostream>
#include <cassert>
#include "FrameInfo.h"
namespace Engine
{

	//TEMP
	struct PushConstantData
	{
		glm::mat4 modelMatrix{ 1.f };
		glm::mat4 normalMatrix{ 1.f };
		
	};

	RendererSystem::RendererSystem(Device& device, VkRenderPass renderpass, VkDescriptorSetLayout globalSetLayout)
		: m_Device(device)
	{
		CreatePipelineLayout(globalSetLayout);
		CreatePipeline(renderpass);
	}
	RendererSystem::~RendererSystem()
	{
		vkDestroyPipelineLayout(m_Device.device(), m_PipelineLayout, nullptr);
	}

	void RendererSystem::CreatePipelineLayout(VkDescriptorSetLayout globalSetLayout)
	{
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(PushConstantData);


		std::vector<VkDescriptorSetLayout> layouts = { globalSetLayout };



		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(layouts.size());
		pipelineLayoutInfo.pSetLayouts = layouts.data();
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

		if (vkCreatePipelineLayout(m_Device.device(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create pipeline layout!");
		}
	}

	void RendererSystem::CreatePipeline(VkRenderPass renderpass)
	{
		assert(m_PipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		PipelineConfigInfo configInfo{};

		Pipeline::DefaultPipelineConfigInfo(configInfo);


		configInfo.renderPass = renderpass;
		configInfo.pipelineLayout = m_PipelineLayout;

		m_Pipeline = std::make_unique<Pipeline>(m_Device, "shaders/simple_shader.vert.spv",
												"shaders/simple_shader.frag.spv",
												configInfo);

		configInfo.Is3D = false;

		m_Pipeline2D = std::make_unique<Pipeline>(m_Device, "shaders/2D_shader.vert.spv",
															"shaders/2D_shader.frag.spv",
															configInfo);
		
	}


	void RendererSystem::RenderGameObjects(FrameInfo& frameinfo, std::vector<GameObject>& gameobj)
	{

		vkCmdBindDescriptorSets
		(
			frameinfo.commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			m_PipelineLayout,
			0, 
			1,
			&frameinfo.globalDescriptorSet,
			0,
			nullptr
		);

		for (auto& obj : gameobj)
		{
			if (obj.Is3D())
			{
				 m_Pipeline->Bind(frameinfo.commandBuffer);

			}
			else
			{
				 m_Pipeline2D->Bind(frameinfo.commandBuffer);
			}

			PushConstantData push{};
			push.modelMatrix = obj.transform.mat4();
			push.normalMatrix = obj.transform.normalMatrix();

			vkCmdPushConstants(frameinfo.commandBuffer, m_PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PushConstantData), &push);

			obj.m_Mesh->Bind(frameinfo.commandBuffer);
			obj.m_Mesh->Draw(frameinfo.commandBuffer);
		}

	}



}
