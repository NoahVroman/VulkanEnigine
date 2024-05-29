#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace Engine
{
	class Camera
	{
	public:
		void SetOrthographic(float left, float right, float bottom, float top, float near, float far);
		void SetPerspective(float fov, float aspect, float near, float far);

		void SetViewDirection(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& up = glm::vec3{0.0f,-1.f,0.0f});
		void SetViewTarget(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up = glm::vec3{0.0f,-1.f,0.0f});
		void SetViewYXZ(const glm::vec3& position, const glm::vec3& rotation);

		const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }

		private:
			glm::mat4 m_ProjectionMatrix{ 1.0f };
			glm::mat4 m_ViewMatrix{ 1.0f };

	};

}
