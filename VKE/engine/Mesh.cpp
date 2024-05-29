#include "Mesh.h"
#include <cassert>
#include <cstring>
#include "utils.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#include <stdexcept>

namespace std{
	template<> struct hash<Engine::Mesh::Vertex> {
		size_t operator()(Engine::Mesh::Vertex const& vertex) const {
			size_t seed = 0;
			Engine::hash_combine(seed, vertex.position,vertex.color,vertex.normal,vertex.uv);
			return seed;
		}
	};
}



namespace Engine
{
	Mesh::Mesh(Device& device, const Builder& builder) : m_Device(device)
	{
		CreateVertexBuffer(builder.vertices);
		CreateIndexBuffer(builder.indices);
	}

	Mesh::~Mesh()
	{
	}

	std::unique_ptr<Mesh> Mesh::CreateModelFromFile(Device& device, const std::string& filepath)
	{
		Builder builder{};
		builder.loadModel(filepath);
		return std::make_unique<Mesh>(device, builder);
	}

	std::unique_ptr<Mesh> Mesh::CreateRectangle(Device& device, float width, float height, const glm::vec2& pos)
	{
		Builder builder{};

		glm::vec3 pos3D{ pos, 0.0f };

		builder.vertices = {
			{pos3D + glm::vec3{ -width / 2.f, -height / 2.f, 0.0f }, {1.0f, 0.0f, 0.0f}, {0.0f,0.0f,1.0f}, {0.0f, 0.0f} },
			{pos3D + glm::vec3{width / 2.f, -height / 2.f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f,0.0f,1.0f}, {1.0f, 0.0f}},
			{pos3D + glm::vec3{width / 2.f, height / 2.f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f,0.0f,1.0f}, {1.0f, 1.0f}},
			{pos3D + glm::vec3{-width / 2.f, height / 2.f, 0.0f}, {1.0f, 1.0f, 0.0f}, {0.0f,0.0f,1.0f}, {0.0f, 1.0f}}
		};

		builder.indices = { 0, 1, 2, 2, 3, 0 };

		return std::make_unique<Mesh>(device, builder);
		
	}

	std::unique_ptr<Mesh> Mesh::CreateOval(Device& device, float width, float height, const glm::vec2& pos)
	{

		Builder builder{};

		constexpr int numSegments = 36;
		constexpr float angleIncrement = (2.0f * glm::pi<float>()) / numSegments;

		
		// Generate vertices for the oval shape
		for (int i = 0; i < numSegments; ++i)
		{
			float angle = i * angleIncrement;
			float x = pos.x + 0.5f * width * cos(angle);
			float y = pos.y + 0.5f * height * sin(angle);
			builder.vertices.push_back({ glm::vec3(x, y, 0.0f) });
		}

		// Generate indices to form triangles connecting the vertices
		for (int i = 1; i < numSegments - 1; ++i)
		{
			builder.indices.push_back(0);         // Center vertex
			builder.indices.push_back(i);         // Current vertex
			builder.indices.push_back(i + 1);     // Next vertex
		}

		// Connect the last vertex to the first and the second vertices
		builder.indices.push_back(0);
		builder.indices.push_back(numSegments - 1);
		builder.indices.push_back(1);

		// Create a new Mesh object with the calculated vertices and indices
		auto mesh = std::make_unique<Mesh>(device, builder);

		// Return the mesh wrapped in a std::unique_ptr
		return mesh;

		
	}


	void Mesh::Bind(VkCommandBuffer commandBuffer)
	{
		VkBuffer buffers[] = { m_VertexBuffer->getBuffer()};
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

		if (m_HasIndexBuffer)
		{
			vkCmdBindIndexBuffer(commandBuffer, m_IndexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
		}
	}

	void Mesh::Draw(VkCommandBuffer commandBuffer)
	{

		if (m_HasIndexBuffer)
		{
			vkCmdDrawIndexed(commandBuffer, m_IndexCount, 1, 0, 0, 0);
		}
		else
		{
			vkCmdDraw(commandBuffer, m_VertexCount, 1, 0, 0);
		}
	}

	void Mesh::CreateVertexBuffer(const std::vector<Vertex>& vertices)
	{
		m_VertexCount = static_cast<uint32_t>(vertices.size());
		assert(m_VertexCount >= 3 && "Vertex count must be at least 3");
		VkDeviceSize bufferSize = sizeof(vertices[0]) * m_VertexCount;
		uint32_t vertexSize = sizeof(vertices[0]);

		Buffer stagingBuffer
		{
			m_Device,
			vertexSize,
			m_VertexCount,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};


		stagingBuffer.map();
		stagingBuffer.writeToBuffer((void*)vertices.data());


		m_VertexBuffer = std::make_unique<Buffer>
			(

				m_Device,
				vertexSize,
				m_VertexCount,
				VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			);
	
		m_Device.copyBuffer(stagingBuffer.getBuffer(), m_VertexBuffer->getBuffer(), bufferSize);
	}

	void Mesh::CreateIndexBuffer(const std::vector<uint32_t>& indices)
	{
		m_IndexCount = static_cast<uint32_t>(indices.size());
		m_HasIndexBuffer = m_IndexCount > 0;



		if (!m_HasIndexBuffer)
			return;

		VkDeviceSize bufferSize = sizeof(indices[0]) * m_IndexCount;
		uint32_t indexSize = sizeof(indices[0]);

		Buffer stagingBuffer
		{
			m_Device,
			indexSize,
			m_IndexCount,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};



		stagingBuffer.map();
		stagingBuffer.writeToBuffer((void*)indices.data());


	
		m_IndexBuffer = std::make_unique<Buffer>
			(

				m_Device,
				indexSize,
				m_IndexCount,
				VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			);
	
	

		m_Device.copyBuffer(stagingBuffer.getBuffer(), m_IndexBuffer->getBuffer(), bufferSize);

	}

	std::vector<VkVertexInputBindingDescription> Mesh::Vertex::GetBindingDescriptions()
	{
		std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
		bindingDescriptions[0].binding = 0;
		bindingDescriptions[0].stride = sizeof(Vertex);
		bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return bindingDescriptions;
	}

	std::vector<VkVertexInputAttributeDescription> Mesh::Vertex::GetAttributeDescriptions(bool Is3D)
	{
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
		if (Is3D)
		{
			attributeDescriptions.push_back({ 0,0,VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position) });

		}
		else
		{
			attributeDescriptions.push_back({ 0,0,VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, position) });
		}
		attributeDescriptions.push_back({1,0,VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color) });
		attributeDescriptions.push_back({2,0,VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal) });
		attributeDescriptions.push_back({3,0,VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv) });


		return attributeDescriptions;
	}


	void Mesh::Builder::loadModel(const std::string& filepath)
	{
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn;
		std::string err;

		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath.c_str()))
		{
			throw std::runtime_error(warn + err);
		}

		vertices.clear();
		indices.clear();


		std::unordered_map<Vertex, uint32_t> uniqueVertices{};

		for (const auto& shape : shapes)
		{
			for (const auto& index : shape.mesh.indices)
			{
				Vertex vertex{};
			
				if (index.vertex_index >= 0)
				{
					vertex.position = {
					attrib.vertices[3 * index.vertex_index + 0],
					attrib.vertices[3 * index.vertex_index + 1],
					attrib.vertices[3 * index.vertex_index + 2]
					};

					auto colorIndex = 3 * index.vertex_index + 2;
					if (colorIndex < attrib.colors.size())
					{
						vertex.color = {

							attrib.colors[colorIndex - 2],
							attrib.colors[colorIndex - 1],
							attrib.colors[colorIndex - 0]

						};
					}
			
			
				}
			
				if (index.normal_index >= 0)
				{
					vertex.normal = {
					attrib.normals[3 * index.normal_index + 0],
					attrib.normals[3 * index.normal_index + 1],
					attrib.normals[3 * index.normal_index + 2]
					};
				}

				if (index.texcoord_index >= 0)
				{
				 	vertex.uv = {
						attrib.texcoords[2 * index.texcoord_index + 0],
						1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
					};
				}
			
			
				
				if (uniqueVertices.count(vertex) == 0)
				{
					uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
					vertices.push_back(vertex);

				}
				indices.push_back(uniqueVertices[vertex]);
			}

		}


	}

};
