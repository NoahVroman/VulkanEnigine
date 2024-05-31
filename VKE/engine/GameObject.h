#pragma once

#include "Mesh.h"

#include <glm/gtc/matrix_transform.hpp>

#include <memory>



namespace Engine
{

	//struct Transform2D
	//{
	//	glm::vec2 translate;
	//	glm::vec2 scale{1.f,1.f};
	//	float rotation{0.f};

	//	glm::mat2 mat2() const
	//	{

	//		glm::mat2 scaleMat = glm::mat2{ {scale.x,0.f},{0.f,scale.y } };
	//		glm::mat2 rotateMat = glm::mat2{ {cos(rotation),sin(rotation)},{-sin(rotation),cos(rotation)} };

	//		return rotateMat * scaleMat;
	//	}
	//};


	struct TransformComponent
	{
		glm::vec3 translate;
		glm::vec3 scale{ 1.f,1.f,1.f };
		glm::vec3 rotation{ 0.f };

		//This optimizes the matrix multiplication so that there are less calculations behind the scenes
		// Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
		// https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
		glm::mat4 mat4();
		glm::mat3 normalMatrix();
	};


	class GameObject
	{
	public:
		using id_t = unsigned int;


		GameObject(const GameObject&) = delete;
		GameObject& operator=(const GameObject&) = delete;
		GameObject(GameObject&&) = default;
		GameObject& operator=(GameObject&&) = default;


		static GameObject Create() 
		{
			static id_t id = 0;
			return GameObject(id++);
		}

		id_t GetId() const { return m_Id; }

		bool IsPBR() const { return isPBR; }
		void SetIsPBR(bool value) { isPBR = value; }


		std::shared_ptr<Mesh> m_Mesh{};
		glm::vec3 color{ };

		TransformComponent transform{};

	private:
		GameObject(id_t id) : m_Id(id) {};

		bool isPBR = true;


		id_t m_Id;
		

	};

}
