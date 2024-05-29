#pragma once

#include "Window.h"
#include "Device.h"
#include "SwapChain.h"

#include <memory>
#include <vector>
#include <cassert>

namespace Engine
{
	class Renderer
	{
	public:
		Renderer(Window& window, Device& device);
		~Renderer();

		Renderer(const Renderer&) = delete;
		void operator=(const Renderer&) = delete;


		bool isFrameComplete() const { return isFrameStarted; }

		VkCommandBuffer GetCurrentCommandBuffer() const { 
			assert(isFrameStarted && "Cannot get command buffer when frame not started");
			return m_CommandBuffers[m_CurrentFrameIndex];
		}

		VkRenderPass GetRenderPass() const { return m_SwapChain->getRenderPass(); }

		float AspectRatio() const { return m_SwapChain->extentAspectRatio(); }


		VkCommandBuffer BeginFrame();
		void EndFrame();

		void BeginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void EndSwapChainRenderPass(VkCommandBuffer commandBuffer) const;

		int GetFrameIndex() const {
			assert(isFrameStarted && "Cannot get frame index when frame not started");
			return m_CurrentFrameIndex; 
		}


	private:

		void CreateCommandBuffers();
		void FreeCommandBuffers();
		void RecreateSwapChain();

		//TODO: MAKE FILE PATHS RELATIVE
		Window& m_Window;
		Device& m_Device;

		std::unique_ptr<SwapChain> m_SwapChain;

		std::vector<VkCommandBuffer> m_CommandBuffers;

		uint32_t m_CurrentImageIndex;
		int m_CurrentFrameIndex{ 0 };
		bool isFrameStarted{ false };


	};

};
