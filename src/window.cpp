#include "window.h"

std::vector<GLFWwindow*> Window::instances;
Window* Window::instance; // default window ptr

Window::Window(std::string title, const glm::ivec2& size, const glm::ivec2& position)
    : width{size.x}
    , height{size.y}
    , title{std::move(title)}
    , position{position}
{
    initGLFW();
    initWindow(false);

    instances.push_back(window);
}

Window::Window(std::string title)
    : width{1}
    , height{1}
    , title{std::move(title)}
    , position{0, 0}
{
    initGLFW();
    initWindow(true);

    instances.push_back(window);
}

Window::~Window() {
    instances.erase(std::remove(instances.begin(), instances.end(), window), instances.end());

    glfwDestroyWindow(window);
    window = nullptr;

    if (instances.empty()) {
        glfwTerminate();
    }
}

void Window::initGLFW() {
    if (instance == this) {
        instance = nullptr;
    }

    if (instances.empty()) {
        int success = glfwInit();
        assert(success && "Failed to initialize GLFW!");
        glfwSetErrorCallback(ErrorCallback);
    }
}

void Window::initWindow(bool fullscreen) {
    std::cout << "Creating window: " << title << " [" << width << " " << height << "]" << std::endl;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    if (fullscreen) {
        auto monitor = glfwGetPrimaryMonitor();
        auto mode = glfwGetVideoMode(monitor);
        width = mode->width;
        height = mode->height;

        window = glfwCreateWindow(width, height, title.c_str(), monitor, nullptr);
    } else {
        window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    }
    assert(window && "Failed to create window!");

    glfwMakeContextCurrent(window);

    int glad = gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
    assert(glad && "Failed to initialize GLAD!");

    glfwSwapInterval(1); // enable vsync
    glViewport(0, 0, width, height);

    if (position != glm::ivec2{ 0, 0 }) {
        glfwSetWindowPos(window, position.x, position.y);
    }

    glfwSetWindowUserPointer(window, this);
    glfwSetWindowPosCallback(window, PosCallback);
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);

    if (instance == nullptr) {
        instance = this;
    }
}

void Window::PosCallback(GLFWwindow* handle, int x, int y) {
    auto& window = *reinterpret_cast<Window *>(glfwGetWindowUserPointer(handle));

    window.position = {x, y};
}

void Window::FramebufferSizeCallback(GLFWwindow* handle, int width, int height) {
    auto& window = *reinterpret_cast<Window *>(glfwGetWindowUserPointer(handle));
    window.width = width;
    window.height = height;
    glViewport(0, 0, width, height);
}

void Window::ErrorCallback(int error, const char* description) {
    std::cerr << "[GLFW] Error (" << error << "): " << description << std::endl;
}