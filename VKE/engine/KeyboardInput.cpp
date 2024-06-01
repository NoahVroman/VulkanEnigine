#include "KeyboardInput.h"
#include "Keyboard.h"

namespace Engine
{

	void KeyboardInput::MoveInXZ(GLFWwindow* window, GameObject& gameObject, float deltaTime)
	{
		
        static double prevXpos = 0, prevYpos = 0;
        double currentXpos, currentYpos;
        glfwGetCursorPos(window, &currentXpos, &currentYpos);

        static bool rightMouseButtonPressed = false;

        // Check if right mouse button is pressed
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
        {
            // Check if right mouse button was pressed in the previous frame
            if (!rightMouseButtonPressed)
            {
                prevXpos = currentXpos;
                prevYpos = currentYpos;
                rightMouseButtonPressed = true;

                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            }
            float deltaX = static_cast<float>(currentXpos - prevXpos);
            float deltaY = static_cast<float>(prevYpos - currentYpos); 

            prevXpos = currentXpos;
            prevYpos = currentYpos;
         
            gameObject.transform.rotation.y += m_Sensitivity * deltaX;
            gameObject.transform.rotation.x += m_Sensitivity * deltaY;

        }
        else
        {
            rightMouseButtonPressed = false;

            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }


        float yaw = gameObject.transform.rotation.y;
        const glm::vec3 forward = glm::vec3(gameObject.transform.mat4() * glm::vec4{ 0, 0, 1,0 });
        const glm::vec3 right = glm::cross(glm::vec3{ 0,1,0 }, forward);

        glm::vec3 Direction = glm::vec3{ 0.f };

        if (glfwGetKey(window, m_keyboardMapping.moveForward) == GLFW_PRESS) Direction += forward;
        if (glfwGetKey(window, m_keyboardMapping.moveBackward) == GLFW_PRESS) Direction -= forward;
        if (glfwGetKey(window, m_keyboardMapping.moveRight) == GLFW_PRESS) Direction += right;
        if (glfwGetKey(window, m_keyboardMapping.moveLeft) == GLFW_PRESS) Direction -= right;

        // Avoid unnecessary normalization
        float directionMagnitude = glm::dot(Direction, Direction);
        if (directionMagnitude > std::numeric_limits<float>::epsilon())
        {
            gameObject.transform.translate += m_MoveSpeed * deltaTime * glm::normalize(Direction);
        }

	}

    void KeyboardInput::CycleRenderMode(GLFWwindow* window, int& renderMode)
    {

        if (Keyboard::getInstance().just_pressed(m_keyboardMapping.cycleRenderMode))
        {
                renderMode += 1;
                renderMode %= 4;
        }
        
    }

}