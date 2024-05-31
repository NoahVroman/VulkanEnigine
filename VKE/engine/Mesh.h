#pragma once
#include "Device.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include "tiny_obj_loader.h"

#include <vector>
#include <memory>

#include "Buffer.h"

namespace Engine
{


	class Mesh
	{
	public:

		struct Vertex
		{
			glm::vec3 position{};
			glm::vec3 color{};
			glm::vec3 normal{};
			glm::vec2 uv{};
			glm::vec3 tangent{};


			static std::vector <VkVertexInputBindingDescription>   GetBindingDescriptions();
			static std::vector <VkVertexInputAttributeDescription> GetAttributeDescriptions(bool isPBR);

			bool operator==(const Vertex& other) const
			{
				return position == other.position && color == other.color && normal == other.normal && uv == other.uv && tangent == other.tangent;
			}
		};

		struct Builder
		{
			std::vector<Vertex> vertices{};
			std::vector<uint32_t> indices{};

			void loadModel(const std::string& filepath);

			void computeTangents();

		};

		Mesh(Device& device, const Builder& builder);
		~Mesh();

		Mesh(const Mesh&) = delete;
		Mesh& operator=(const Mesh&) = delete;

		static std::unique_ptr<Mesh> CreateModelFromFile(Device& device, const std::string& filepath);

		static std::unique_ptr<Mesh> CreateRectangle(Device& device, float width, float height, const glm::vec2& pos);

		static std::unique_ptr<Mesh> CreateOval(Device& device, float width, float height, const glm::vec2& pos);

		void Bind(VkCommandBuffer commandBuffer);
		void Draw(VkCommandBuffer commandBuffer);

		uint32_t GetVertexCount() const { return m_VertexCount; }


	private:

		void CreateVertexBuffer(const std::vector<Vertex>& vertices);
		void CreateIndexBuffer(const std::vector<uint32_t>& indices);



		Device& m_Device;

		std::unique_ptr<Buffer> m_VertexBuffer;
		uint32_t m_VertexCount;
		
		bool m_HasIndexBuffer{false};

		std::unique_ptr<Buffer> m_IndexBuffer;
		uint32_t m_IndexCount;




	};
}

