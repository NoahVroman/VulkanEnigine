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

#include "Keyboard.h"

namespace Engine
{
	struct alignas(16) GlobalUbo {
		glm::mat4 projectionVieuw{ 1.f };
		alignas(16) glm::vec3 lightDirection = glm::normalize(glm::vec3{ 0.5f, 0.8f, 0.5f });
		alignas(16) glm::vec3 cameraPosition = { 0, 0, 0 };
		alignas(4) int renderMode{};
	};


	App::App()
	{

		

		m_AlbedoTexture = std::make_unique<Image>(m_Device, "models/vehicle_diffuse.png");
		m_NormalTexture = std::make_unique<Image>(m_Device, "models/vehicle_normal.png");
		m_SpecularTexture = std::make_unique<Image>(m_Device, "models/vehicle_specular.png");
		m_RoughnessTexture = std::make_unique<Image>(m_Device, "models/vehicle_roughness.png");


		m_GlobalPool = DescriptorPool::Builder(m_Device)
			.setMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, SwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, SwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, SwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, SwapChain::MAX_FRAMES_IN_FLIGHT)
			.build();

		LoadGameObjects();

	}
	App::~App()
	{
		m_GameObjects.clear();

		vkDeviceWaitIdle(m_Device.device());
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
			.addBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.addBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.addBinding(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.addBinding(4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.build();

		std::vector<VkDescriptorSet> globalDescriptorSets(SwapChain::MAX_FRAMES_IN_FLIGHT);
		for (size_t i = 0; i < globalDescriptorSets.size(); i++)
		{
			VkDescriptorImageInfo diffuseinfo;
			diffuseinfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			diffuseinfo.imageView = m_AlbedoTexture->GetTextureImageView();
			diffuseinfo.sampler = m_AlbedoTexture->GetTextureSampler();

			VkDescriptorImageInfo normal;
			normal.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			normal.imageView = m_NormalTexture->GetTextureImageView();
			normal.sampler = m_NormalTexture->GetTextureSampler();

			VkDescriptorImageInfo specular;
			specular.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			specular.imageView = m_SpecularTexture->GetTextureImageView();
			specular.sampler = m_SpecularTexture->GetTextureSampler();

			VkDescriptorImageInfo roughness;
			roughness.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			roughness.imageView = m_RoughnessTexture->GetTextureImageView();
			roughness.sampler = m_RoughnessTexture->GetTextureSampler();
			

			auto bufferInfo = uboBuffer[i]->descriptorInfo();
			DescriptorWriter(*globalSetLayout,*m_GlobalPool)
				.writeBuffer(0, &bufferInfo)
				.writeImage(1, &diffuseinfo)
				.writeImage(2, &normal)
				.writeImage(3, &specular)
				.writeImage(4, &roughness)
				.build(globalDescriptorSets[i]);
		}

		RendererSystem rendererSystem(m_Device, m_Renderer.GetRenderPass(),globalSetLayout->getDescriptorSetLayout());
		Camera camera{};

		auto vieuwerObject = GameObject::Create();
		vieuwerObject.transform.translate = { 8.f,0.f,-40.f };

		KeyboardInput keyboardInput{};
		auto currentTime = std::chrono::high_resolution_clock::now();

		GlobalUbo globalUbo{};
		int renderMode{3};



		while (!m_Window.ShouldClose())
		{
			glfwPollEvents();

			Keyboard::getInstance().update(m_Window.GetWindow());

			keyboardInput.CycleRenderMode(m_Window.GetWindow(), renderMode);


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

				globalUbo.projectionVieuw = camera.GetProjectionMatrix() * camera.GetViewMatrix();
				globalUbo.cameraPosition = vieuwerObject.transform.translate;
				globalUbo.renderMode = renderMode;
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

		std::shared_ptr<Mesh> vehicleMesh = Mesh::CreateModelFromFile(m_Device, "models/vehicle.obj");


		auto vehicle = GameObject::Create();
		vehicle.m_Mesh = vehicleMesh;
		vehicle.transform.translate = { 1.f,0.f,0.f };
		vehicle.transform.scale = { 0.5f,0.5f,0.5f };
		vehicle.transform.rotation = { 3.14f,0.f,0.f };
		vehicle.SetIsPBR(true);
	
		m_GameObjects.push_back(std::move(vehicle));


		std::shared_ptr<Mesh> vehicleMesh2 = Mesh::CreateModelFromFile(m_Device, "models/vehicle.obj");


		auto vehicle2 = GameObject::Create();
		vehicle2.m_Mesh = vehicleMesh2;
		vehicle2.transform.translate = { -18.f,0.f,0.f };
		vehicle2.transform.scale = { 0.5f,0.5f,0.5f };
		vehicle2.transform.rotation = { 3.14f,0.f,0.f };
		vehicle2.SetIsPBR(true);

		m_GameObjects.push_back(std::move(vehicle2));


		std::shared_ptr<Mesh> colored_cubeMesh = Mesh::CreateModelFromFile(m_Device, "models/colored_cube.obj");

		auto colored_cube = GameObject::Create();
		colored_cube.m_Mesh = colored_cubeMesh;
		colored_cube.transform.translate = { 30.f,0.f,0.f };
		colored_cube.transform.scale = { 2.f,2.f,2.f };
		colored_cube.transform.rotation = { 3.14f,0.f,0.f };
		colored_cube.SetIsPBR(false);
		
		m_GameObjects.push_back(std::move(colored_cube));

		std::shared_ptr<Mesh> viking_room = Mesh::CreateModelFromFile(m_Device, "models/viking_room.obj");

		auto viking_roomOBJ = GameObject::Create();
		viking_roomOBJ.m_Mesh = viking_room;
		viking_roomOBJ.transform.translate = { 25.f,0.f,0.f };
		viking_roomOBJ.transform.scale = { 2.f,2.f,2.f };
		viking_roomOBJ.transform.rotation = { 3.14f/2.f,3.14/2.f,0.f };
		viking_roomOBJ.SetIsPBR(false);

		m_GameObjects.push_back(std::move(viking_roomOBJ));


		std::shared_ptr<Mesh> sphere = Mesh::CreateModelFromFile(m_Device, "models/sphere.obj");

		auto sphereObject = GameObject::Create();
		sphereObject.m_Mesh = sphere;
		sphereObject.transform.translate = { 15.f,0.f,0.f };
		sphereObject.transform.scale = { 1.f,1.f,1.f };
		sphereObject.transform.rotation = { 0.f,0.f,0.f };
		sphereObject.SetIsPBR(false);

		m_GameObjects.push_back(std::move(sphereObject));



		std::shared_ptr<Mesh> sphere2 = Mesh::CreateModelFromFile(m_Device, "models/sphere.obj");

		auto sphereObject2 = GameObject::Create();
		sphereObject2.m_Mesh = sphere2;
		sphereObject2.transform.translate = { 20.f,0.f,0.f };
		sphereObject2.transform.scale = { 1.f,1.f,1.f };
		sphereObject2.transform.rotation = { 0.f,0.f,0.f };
		sphereObject2.SetIsPBR(false);

		m_GameObjects.push_back(std::move(sphereObject2));

	}

}
