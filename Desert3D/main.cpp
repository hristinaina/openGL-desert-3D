#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <sstream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "helper.h"
#include "desert.h"

/*  TODO
* 1. prebaciti u sejder fajlove
* 2. kreirati helper
* 3. definisati posebne fajlove za piramidu
* 4. iskoristiti postojeci kod ali prosiriti model matricom 
*/

const GLuint WIDTH = 800, HEIGHT = 600;


void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

GLFWwindow* initWindow() {
    if (!glfwInit()) {
        std::cout << "GLFW Initialization Failed!" << std::endl;
        return nullptr;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Pyramids - Bird's Eye View", nullptr, nullptr);
    if (!window) {
        std::cout << "GLFW Window Creation Failed!" << std::endl;
        glfwTerminate();
        return nullptr;
    }

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cout << "GLEW Initialization Failed!" << std::endl;
        glfwTerminate();
        return nullptr;
    }

    glViewport(0, 0, WIDTH, HEIGHT);

    return window;
}

int main() {
    GLFWwindow* window = initWindow();
    if (!window) return -1;

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Create objects
    createPyramid();

    // Compile shaders
    unsigned int basicShader = createShader("basic.vert", "basic.frag");

    // Create MVP matrix
    glm::mat4 projection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 0.1f, 100.0f);
    glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 5.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 model = glm::mat4(1.0f);

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.337, 0.451, 0.51, 1.0);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Input actions

        glm::mat4 MVP = projection * view * model;
        renderPyramid(basicShader, MVP);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteProgram(basicShader);

    glfwTerminate();
    return 0;
}
