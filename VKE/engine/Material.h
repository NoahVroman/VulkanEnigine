#pragma once
#include "vulkan/vulkan.h"
#include "Pipeline.h"
#include "Mesh.h"
#include "Image.h"
#include <memory>
#include "Descriptors.h"
namespace Engine
{
	class Material
	{
		public:
		Material(const Device& device,const std::string& albedo, const std::string& normal, const std::string& roughness, const std::string& specular);
		~Material() = default;
		Material(const Material&) = delete;
		Material& operator=(const Material&) = delete;

		void Draw(VkCommandBuffer commandBuffer,Pipeline* pipeline,Mesh* Mesh);

		private:

		std::unique_ptr<Image> m_AlbedoTexture;
		std::unique_ptr<Image> m_NormalTexture;
		std::unique_ptr<Image> m_RoughnessTexture;
		std::unique_ptr<Image> m_Specular;
			
	};
}

