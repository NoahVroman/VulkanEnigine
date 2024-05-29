#include "App.h"
#include "RendererSystem.h"
#include "Camera.h"
#include "KeyboardInput.h"
#include "Buffer.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>


#include <stdexcept>
#include <array>
#include <iostream>
#include <cassert>
#include <chrono>
#include "FrameInfo.h"

namespace Engine
{
	struct GlobalUbo
	{
	    alignas(16)	glm::mat4 projectionVieuw{ 1.f };
		alignas(16)	glm::vec3 lightDirection = glm::normalize(glm::vec3{ 1.f,-3.f,-1.f });
	};


	App::App()
	{
		m_GlobalPool = DescriptorPool::Builder(m_Device)
			.setMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,SwapChain::MAX_FRAMES_IN_FLIGHT)
			.build();

		LoadGameObjects();

	}
	App::~App()
	{

	}
	void App::run()
	{

		std::vector < std::unique_ptr<Buffer>> uboBuffer(SwapChain::MAX_FRAMES_IN_FLIGHT);

		for (size_t i = 0; i < uboBuffer.size(); i++)
		{
			uboBuffer[i] = std::make_unique<Buffer>
				(
					m_Device,
					sizeof(GlobalUbo),
					1,
					VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
				);
			uboBuffer[i]->map();
		}

		auto globalSetLayout = DescriptorSetLayout::Builder(m_Device)
			.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
			.build();

		std::vector<VkDescriptorSet> globalDescriptorSets(SwapChain::MAX_FRAMES_IN_FLIGHT);
		for (size_t i = 0; i < globalDescriptorSets.size(); i++)
		{
			auto bufferInfo = uboBuffer[i]->descriptorInfo();
			DescriptorWriter(*globalSetLayout,*m_GlobalPool)
				.writeBuffer(0, &bufferInfo)
				.build(globalDescriptorSets[i]);
		}

		RendererSystem rendererSystem(m_Device, m_Renderer.GetRenderPass(),globalSetLayout->getDescriptorSetLayout());
		Camera camera{};
		camera.SetViewTarget(glm::vec3{ -1.f,-2.f,2.f }, glm::vec3{ 0.f,0.f,2.5f });

		auto vieuwerObject = GameObject::Create();
		KeyboardInput keyboardInput{};



		auto currentTime = std::chrono::high_resolution_clock::now();


		while (!m_Window.ShouldClose())
		{
			glfwPollEvents();


			auto newTime = std::chrono::high_resolution_clock::now();
			float deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
			currentTime = newTime;


			keyboardInput.MoveInXZ(m_Window.GetWindow(), vieuwerObject, deltaTime);
			camera.SetViewYXZ(vieuwerObject.transform.translate, vieuwerObject.transform.rotation);

			float aspectRatio = m_Renderer.AspectRatio();
			camera.SetPerspective(glm::radians(45.f), aspectRatio, 0.1f, 100.f);


			if (auto commandbuffer = m_Renderer.BeginFrame())
			{
				int frameindex = m_Renderer.GetFrameIndex();

				FrameInfo frameInfo
				{
					frameindex,
					deltaTime,
					commandbuffer,
					camera,
					globalDescriptorSets[frameindex]
				
				};

				//update
				GlobalUbo globalUbo{};
				globalUbo.projectionVieuw = camera.GetProjectionMatrix() * camera.GetViewMatrix();
				uboBuffer[frameindex]->writeToBuffer(&globalUbo);
				uboBuffer[frameindex]->flush();

				//render
				m_Renderer.BeginSwapChainRenderPass(commandbuffer);
				rendererSystem.RenderGameObjects(frameInfo,m_GameObjects);
				m_Renderer.EndSwapChainRenderPass(commandbuffer);
				m_Renderer.EndFrame();

			}
		}

		vkDeviceWaitIdle(m_Device.device());

	}

	void App::LoadGameObjects()
	{
		std::shared_ptr<Mesh> mesh = Mesh::CreateRectangle(m_Device, 0.3f, 0.3f,glm::vec2{-0.7,0.7});

		auto meshObj = GameObject::Create();
		meshObj.m_Mesh = mesh;
		meshObj.transform.translate = { 0.1f,0.f,0.f };
		meshObj.transform.scale = { 0.3f,0.3,0.3f };
		meshObj.transform.rotation = { 3.14f / 2.f ,3.14f / 2.f ,0.f};
		meshObj.SetIs3D(false);

		m_GameObjects.push_back(std::move(meshObj));


		std::shared_ptr<Mesh> OvalMesh = Mesh::CreateOval(m_Device, 0.3f, 0.3f, glm::vec2{ -0.3,0.7 });

		auto ovalObj = GameObject::Create();
		ovalObj.m_Mesh = OvalMesh;
		ovalObj.transform.translate = { 0.1f,0.f,0.f };
		ovalObj.transform.scale = { 0.3f,0.3,0.3f };
		ovalObj.transform.rotation = { 3.14f / 2.f ,3.14f / 2.f ,0.f };
		ovalObj.SetIs3D(false);

		m_GameObjects.push_back(std::move(ovalObj));


		std::shared_ptr<Mesh> Secondmesh = Mesh::CreateModelFromFile(m_Device, "models/colored_cube.obj");


		auto meshObj2 = GameObject::Create();
		meshObj2.m_Mesh = Secondmesh;
		meshObj2.transform.translate = { 1.f,0.f,2.5f };
		meshObj2.transform.scale = { 0.5f,0.5f,0.5f };

		m_GameObjects.push_back(std::move(meshObj2));

	}

}
