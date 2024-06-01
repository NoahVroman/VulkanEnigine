#pragma once

#include "GameObject.h"
#include "Window.h"

namespace Engine 
{
	class KeyboardInput
	{
	public:

		//TODO: ADD LOOKING WITH MOUSE
		struct KeyboardMapping
		{
			int moveLeft = GLFW_KEY_A;
			int moveRight = GLFW_KEY_D;
			int moveForward = GLFW_KEY_W;
			int moveBackward = GLFW_KEY_S;	

			int cycleRenderMode = GLFW_KEY_R;

			int Look = GLFW_MOUSE_BUTTON_LEFT;
		};


		void MoveInXZ(GLFWwindow* window, GameObject& gameObject, float deltaTime);

		void CycleRenderMode(GLFWwindow* window, int& renderMode);


	private:
		KeyboardMapping m_keyboardMapping;
		float m_MoveSpeed{ 15.f };
		float m_Sensitivity{ 0.001f };

		
	};

};
