#pragma once

#include "Window.h"
#include "Device.h"
#include "GameObject.h"
#include "Renderer.h"
#include "Descriptors.h"


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

	

		std::vector<GameObject> m_GameObjects;

	};

};
