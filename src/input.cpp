#include "input.h"

bool Input::keys[]{};
uint32_t Input::frames[]{};
uint32_t Input::current{};
glm::vec2 Input::delta{};
glm::vec2 Input::scroll{};
glm::vec2 Input::position{};

void Input::Init(const Window& window) {
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    glfwSetScrollCallback(window, ScrollCallback);
    glfwSetCursorPosCallback(window, CursorPositionCallback);
}

bool Input::GetKey(int keycode) {
    if (keycode < 0 || keycode >= MOUSE_BUTTONS)
        return false;
    return keys[keycode];
}

bool Input::GetKeyDown(int keycode) {
    if (keycode < 0 || keycode >= MOUSE_BUTTONS)
        return false;
    return keys[keycode] && frames[keycode] == current;
}

bool Input::GetMouseButton(int button) {
    int index = MOUSE_BUTTONS + button;
    return keys[index];
}

bool Input::GetMouseButtonDown(int button) {
    int index = MOUSE_BUTTONS + button;
    return keys[index] && frames[index] == current;
}

void Input::Update() {
    current++;
    delta = {};
    scroll = {};
}

glm::vec2& Input::MousePosition() {
    return position;
}

glm::vec2& Input::MouseDelta() {
    return delta;
}

glm::vec2& Input::MouseScroll() {
    return scroll;
}

void Input::CursorPositionCallback(GLFWwindow* window, double mouseX, double mouseY) {
    glm::vec2 mouse {mouseX, mouseY};
    delta += mouse - position;
    position = mouse;
}

void Input::ScrollCallback(GLFWwindow* window, double offsetX, double offsetY) {
    glm::vec2 offset {offsetX, offsetY};
    scroll = offset;
}

void Input::MouseButtonCallback(GLFWwindow* window, int button, int action, int mode) {
    int key = MOUSE_BUTTONS + button;
    if (action == GLFW_PRESS) {
        keys[key] = true;
        frames[key] = current;
    } else if (action == GLFW_RELEASE) {
        keys[key] = false;
        frames[key] = current;
    }
}

void Input::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if (action == GLFW_PRESS) {
        keys[key] = true;
        frames[key] = current;
    } else if (action == GLFW_RELEASE) {
        keys[key] = false;
        frames[key] = current;
    }
}