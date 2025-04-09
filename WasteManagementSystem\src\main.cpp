// main.cpp
// Entry point for the Waste Management System application
#include "Application.h"
#include "UI/UIManager.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

// GLFW callback function for error handling
void glfw_error_callback(int error, const char* description) {
    std::cerr << "GLFW Error " << error << ": " << description << std::endl;
}

// GLFW callback function for window resizing
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);

    // Update UI manager with new size
    Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
    if (app && app->GetUIManager()) {
        app->GetUIManager()->HandleResize(width, height);
    }
}

int main(int argc, char** argv) {
    // Set up GLFW error callback
    glfwSetErrorCallback(glfw_error_callback);

    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Set OpenGL version and profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Enable multi-sampling for anti-aliasing
    glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Create window
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Waste Management System", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Make OpenGL context current
    glfwMakeContextCurrent(window);

    // Set vsync
    glfwSwapInterval(1);

    // Set framebuffer resize callback
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Enable anti-aliasing
    glEnable(GL_MULTISAMPLE);

    // Initialize application
    Application app;

    // Store application pointer in window user pointer
    glfwSetWindowUserPointer(window, &app);

    // Initialize the application
    if (!app.Initialize(window)) {
        std::cerr << "Failed to initialize application" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Main application loop
    app.Run();

    // Shutdown application
    app.Shutdown();

    // Clean up GLFW
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
