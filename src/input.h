#pragma once

#include "window.h"

#define MOUSE_BUTTONS 500

class Input {
private:
    static bool keys[512];
    static uint32_t frames[512];
    static uint32_t current;
    static glm::vec2 delta;
    static glm::vec2 scroll;
    static glm::vec2 position;

public:
    static void Init(const Window& window);
    static void Update();

    static bool GetKey(int keycode);
    static bool GetKeyDown(int keycode);

    static bool GetMouseButton(int button);
    static bool GetMouseButtonDown(int button);

    static glm::vec2& MousePosition();
    static glm::vec2& MouseDelta();
    static glm::vec2& MouseScroll();

private:
    static void CursorPositionCallback(GLFWwindow* window, double mouseX, double mouseY);
    static void ScrollCallback(GLFWwindow* window, double offsetX, double offsetY);
    static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mode);
    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
};
