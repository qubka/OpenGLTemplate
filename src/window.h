#pragma once

class Window {
public:
    Window(std::string title, const glm::ivec2& size, const glm::ivec2& position = {});
    Window(std::string title);
    ~Window();

    static Window& GetInstance() { return *instance; }

    operator GLFWwindow*() const { return window; }
    int GetWidth() const { return width; }
    int GetHeight() const { return height; }
    float GetAspect() const { return static_cast<float>(width) / static_cast<float>(height); }
    glm::vec4 GetViewport() const { return {0, height, width, -height}; } // vertical flip is required

    bool ShouldClose() const { return glfwWindowShouldClose(window); }
    void ShouldClose(bool flag) const { glfwSetWindowShouldClose(window, flag); }

    void MakeCurrent() const { glfwMakeContextCurrent(window); }
    void SwapBuffers() const { glfwSwapBuffers(window); }

    void ShowWindow(bool show = true) {
        if (show) {
            glfwShowWindow(window);
        } else {
            glfwHideWindow(window);
        }
    }

    void SetCursorPos(const glm::dvec2& pos) {
        glfwSetCursorPos(window, pos.x, pos.y);
    }

    const std::string& GetTitle() const { return title; }
    void SetTitle(const std::string& str) {
        title = str;
        glfwSetWindowTitle(window, title.c_str());
    }

    const glm::ivec2& GetPosition() const { return position; }
    void SetSizeLimits(const glm::uvec2& minSize, const glm::uvec2& maxSize = {}) {
        glfwSetWindowSizeLimits(window, minSize.x, minSize.y, (maxSize.x != 0) ? maxSize.x : minSize.x, (maxSize.y != 0) ? maxSize.y : minSize.y);
    }

    void PollEvents() const {
        glfwPollEvents();
    }

    void WaitEvents() const {
        glfwWaitEvents();
    }

    void ToggleCursor() {
        locked = !locked;
        glfwSetInputMode(window, GLFW_CURSOR, locked ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
    }

    void ToggleWireframe() {
        wireframe = !wireframe;
        glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
    }

    bool Locked() const { return locked; }
    bool Wireframe() const { return wireframe; }

private:
    GLFWwindow* window;
    int width;
    int height;
    std::string title;
    glm::ivec2 position;

    bool locked{ false };
    bool wireframe{ false };

    void initGLFW();
    void initWindow(bool fullscreen);

    static Window* instance;
    static std::vector<GLFWwindow*> instances;

    static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
    static void PosCallback(GLFWwindow* window, int x, int y);
    static void ErrorCallback(int error, const char* description);
};
