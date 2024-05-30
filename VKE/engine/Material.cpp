#include "Material.h"

Engine::Material::Material(const Device& device,
						   const std::string& albedo,
						   const std::string& normal,
						   const std::string& roughness,
						   const std::string& specular)
{
	//m_AlbedoTexture = std::make_unique<Image>(device, albedo);
	//m_NormalTexture = std::make_unique<Image>(device, normal);
	//m_RoughnessTexture = std::make_unique<Image>(device, roughness);
	//m_Specular = std::make_unique<Image>(device, specular);

}

void Engine::Material::Draw(VkCommandBuffer commandBuffer, Pipeline* pipeline, Mesh* Mesh)
{
	pipeline->Bind(commandBuffer);
	Mesh->Bind(commandBuffer);
	//BIND TEXTURES WITH MESH
	Mesh->Draw(commandBuffer);



}
