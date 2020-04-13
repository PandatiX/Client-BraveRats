#ifndef CLIENTBRAVERATS_WINDOW_HPP
#define CLIENTBRAVERATS_WINDOW_HPP

#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Window {
private:
    GLFWwindow* handle;

public:
    Window(const std::string&, int, int);

    void swapBuffers();
    void close();
    bool shouldClose() const;
    int getKey(int keyCode) const;
    GLFWwindow* getHandle() const;

    ~Window();
};

#endif //CLIENTBRAVERATS_WINDOW_HPP