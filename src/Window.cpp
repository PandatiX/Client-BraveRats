#include "Window.hpp"
#include "exceptions/WindowException.hpp"

Window::Window(std::string title, int height, int width) {

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    handle = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

    glfwMakeContextCurrent(handle);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)){

        glfwTerminate();

        throw WindowException("Failed to initialize GLAD.");

    }

    glViewport(0, 0, width, height);
    glClearColor(0.45f, 0.55f, 0.60f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void Window::swapBuffers() {
    glfwSwapBuffers(handle);
}

void Window::close() {
    glfwSetWindowShouldClose(handle, true);
}

bool Window::shouldClose() const {
    return glfwWindowShouldClose(handle);
}

int Window::getKey(int keyCode) const {
    return glfwGetKey(handle, keyCode);
}
GLFWwindow* Window::getHandle() const {
    return handle;
}


Window::~Window() {
    glfwTerminate();
}