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

GLFWwindow* initWindow() {
    if (!glfwInit())
    {
        std::cout << "GLFW Biblioteka se nije ucitala! :(\n";
        exit(1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window;
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    window = glfwCreateWindow(mode->width, mode->height, "Desert", monitor, NULL);
    if (window == NULL)
    {
        std::cout << "Prozor nije napravljen! :(\n";
        glfwTerminate();
        exit(2);
    }

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
    {
        std::cout << "GLEW nije mogao da se ucita! :'(\n";
        exit(3);
    }

    return window;
}

//todo adjust parameters
void setLight(unsigned int lightingShader) {
    //material
    glUniform3f(glGetUniformLocation(lightingShader, "material.ambient"), 1.0f, 0.5f, 0.31f);
    glUniform3f(glGetUniformLocation(lightingShader, "material.diffuse"), 1.0f, 0.5f, 0.31f);
    glUniform3f(glGetUniformLocation(lightingShader, "material.specular"), 0.5f, 0.5f, 0.5f);
    glUniform1f(glGetUniformLocation(lightingShader, "material.shininess"), 32.0f);
    // directional light
    glUniform3f(glGetUniformLocation(lightingShader, "dirLight.direction"), -0.2f, -1.0f, -0.3f);
    glUniform3f(glGetUniformLocation(lightingShader, "dirLight.ambient"), 0.05f, 0.05f, 0.05f);
    glUniform3f(glGetUniformLocation(lightingShader, "dirLight.diffuse"), 0.4f, 0.4f, 0.4f);
    glUniform3f(glGetUniformLocation(lightingShader, "dirLight.specular"), 0.5f, 0.5f, 0.5f);
    // point light 1
    glUniform3f(glGetUniformLocation(lightingShader, "pointLights[0].position"), 0.7f, 0.2f, 2.0f);
    glUniform3f(glGetUniformLocation(lightingShader, "pointLights[0].ambient"), 0.05f, 0.05f, 0.05f);
    glUniform3f(glGetUniformLocation(lightingShader, "pointLights[0].diffuse"), 0.8f, 0.8f, 0.8f);
    glUniform3f(glGetUniformLocation(lightingShader, "pointLights[0].specular"), 1.0f, 1.0f, 1.0f);
    glUniform1f(glGetUniformLocation(lightingShader, "pointLights[0].constant"), 1.0f);
    glUniform1f(glGetUniformLocation(lightingShader, "pointLights[0].linear"), 0.09f);
    glUniform1f(glGetUniformLocation(lightingShader, "pointLights[0].quadratic"), 0.032f);
    // point light 2
    glUniform3f(glGetUniformLocation(lightingShader, "pointLights[1].position"), 2.3f, -3.3f, -4.0f);
    glUniform3f(glGetUniformLocation(lightingShader, "pointLights[1].ambient"), 0.05f, 0.05f, 0.05f);
    glUniform3f(glGetUniformLocation(lightingShader, "pointLights[1].diffuse"), 0.8f, 0.8f, 0.8f);
    glUniform3f(glGetUniformLocation(lightingShader, "pointLights[1].specular"), 1.0f, 1.0f, 1.0f);
    glUniform1f(glGetUniformLocation(lightingShader, "pointLights[1].constant"), 1.0f);
    glUniform1f(glGetUniformLocation(lightingShader, "pointLights[1].linear"), 0.09f);
    glUniform1f(glGetUniformLocation(lightingShader, "pointLights[1].quadratic"), 0.032f);
    // point light 3
    glUniform3f(glGetUniformLocation(lightingShader, "pointLights[2].position"), -4.0f, 2.0f, -12.0f);
    glUniform3f(glGetUniformLocation(lightingShader, "pointLights[2].ambient"), 0.05f, 0.05f, 0.05f);
    glUniform3f(glGetUniformLocation(lightingShader, "pointLights[2].diffuse"), 0.8f, 0.8f, 0.8f);
    glUniform3f(glGetUniformLocation(lightingShader, "pointLights[2].specular"), 1.0f, 1.0f, 1.0f);
    glUniform1f(glGetUniformLocation(lightingShader, "pointLights[2].constant"), 1.0f);
    glUniform1f(glGetUniformLocation(lightingShader, "pointLights[2].linear"), 0.09f);
    glUniform1f(glGetUniformLocation(lightingShader, "pointLights[2].quadratic"), 0.032f);

    glUniform3f(glGetUniformLocation(lightingShader, "viewPos"), 0.0f, 15.0f, 5.0f); //todo adjust with view
    // spotLight
    //glUniform3f(glGetUniformLocation(lightingShader, "spotLight.position"), 0.0f, 5.0f, 5.0f); //todo adjust with view
    //glUniform3f(glGetUniformLocation(lightingShader, "spotLight.direction"), 0.0f, 0.0f, 0.0f);
    //glUniform3f(glGetUniformLocation(lightingShader, "spotLight.ambient"), 0.0f, 0.0f, 0.0f);
    //glUniform3f(glGetUniformLocation(lightingShader, "spotLight.diffuse"), 1.0f, 1.0f, 1.0f);
    //glUniform3f(glGetUniformLocation(lightingShader, "spotLight.specular"), 1.0f, 1.0f, 1.0f);
    //glUniform1f(glGetUniformLocation(lightingShader, "spotLight.constant"), 1.0f);
    //glUniform1f(glGetUniformLocation(lightingShader, "spotLight.linear"), 0.09f);
    //glUniform1f(glGetUniformLocation(lightingShader, "spotLight.quadratic"), 0.032f);
    //glUniform1f(glGetUniformLocation(lightingShader, "spotLight.cutOff"), glm::cos(glm::radians(12.5f)));
    //glUniform1f(glGetUniformLocation(lightingShader, "spotLight.outerCutOff"), glm::cos(glm::radians(15.0f)));

}

int main() {
    GLFWwindow* window = initWindow();
    if (!window) return -1;

    // Create objects
    createPyramids();

    // Compile shaders
    unsigned int basicShader = createShader("basic.vert", "basic.frag");

    // Create MVP matrices
    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    glm::mat4 projectionP = glm::perspective(glm::radians(45.0f), (float)mode->width / (float)mode->height, 0.1f, 100.0f);
    glm::mat4 projectionO = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 0.1f, 100.0f);
    //glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
    glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 15.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 project = projectionP;

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.337, 0.451, 0.51, 1.0);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Input actions
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
        glUseProgram(basicShader);
        setLight(basicShader);
        glUseProgram(0);

        renderPyramids(basicShader, view, project);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteProgram(basicShader);

    glfwTerminate();
    return 0;
}
