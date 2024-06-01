#include <GLFW/glfw3.h>
#include <unordered_map>


namespace Engine {
    class Keyboard {
    public:



        static Keyboard& getInstance() {
            static Keyboard instance;
            return instance;
        }

        void update(GLFWwindow* wnd) {
            for (auto& [key, state] : _key_states) {
                bool is_currently_pressed = (glfwGetKey(wnd, key) == GLFW_PRESS);
                state.just_pressed = (!state.is_pressed && is_currently_pressed);
                state.just_released = (state.is_pressed && !is_currently_pressed);
                state.is_pressed = is_currently_pressed;
            }
        }

        bool is_pressed(int key) const {
            return _key_states.at(key).is_pressed;
        }

        bool just_pressed(int key) const {
            return _key_states.at(key).just_pressed;
        }

        bool just_released(int key) const {
            return _key_states.at(key).just_released;
        }

    private:
        struct key_state {
            bool is_pressed;
            bool just_pressed;
            bool just_released;
        };

        std::unordered_map<int, key_state> _key_states;

        Keyboard()
        {
            for (int i = GLFW_KEY_SPACE; i <= GLFW_KEY_LAST; ++i) {
                _key_states[i] = { false, false, false };
            }
        }

    };
}