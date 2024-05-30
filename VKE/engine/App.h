#pragma once

#include "Window.h"
#include "Device.h"
#include "GameObject.h"
#include "Renderer.h"
#include "Descriptors.h"
#include "Image.h"


#include <memory>
#include <vector>

namespace Engine 
{
	class App
	{
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;
		App();
		~App();

		App(const App&) = delete;
		App& operator=(const App&) = delete;

		void run();


	private:

		void LoadGameObjects();


		//TODO: MAKE FILE PATHS RELATIVE
		Window m_Window{"VULKAN",WIDTH,HEIGHT};
		Device m_Device{m_Window};
		Renderer m_Renderer{ m_Window,m_Device };

		std::unique_ptr<DescriptorPool> m_GlobalPool{};

		Image m_AlbedoTexture{ m_Device,"models/vehicle_diffuse.png" };
		Image m_NormalTexture{ m_Device,"models/vehicle_normal.png" };
		Image m_SpecularTexture{ m_Device,"models/vehicle_specular.png" };
		Image m_RoughnessTexture{ m_Device,"models/vehicle_roughness.png" };

		std::vector<GameObject> m_GameObjects;

	};

};
