#include <iostream>
#include <stdlib.h>
#include <glad/glad.h> 
#include <cmath>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "main.h"

//  Window Settings
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 1200;


int main() {

    /* Initialize GLFW and window */
    GLFWwindow* window = InitializeGLFW();
    if (!window) {
        return -1;
    }

    /* Bind callbacks and input */
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
  
    // Capture mouse inputs using GLFW
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    /*
     * Main rendering loop
     */
    while (!glfwWindowShouldClose(window))
    {
        /* Swap buffers and poll events */
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 1;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    (void) window; // Unusued parameter
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    /* Unused window parameter */
    (void) window;
    (void) xposIn;
    (void) yposIn;
}

GLFWwindow* InitializeGLFW()
{
    /* Initialize GLFW*/
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Initialize window */
    GLFWwindow* newWindow = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Piranesi", NULL, NULL);
    /* Check for error during initialization */
    if (newWindow == NULL)
    {
        std::cout << "Error when creating window" << std::endl;
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(newWindow);
    
    return newWindow;
}