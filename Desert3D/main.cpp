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
* 9. tackasto svjetlo: testirati promjenom dometa i postaviti objekat na tu poziciju (sa obicnim sejderom tj da ima mvp)
* 10. teksture... dodati ime i br indeksa
*/

float cameraSpeed = 0.03f;

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

void setLight(unsigned int lightingShader, glm::vec3 cameraTranslation) {
    glm::vec3 pyramidPositions[] = {
    glm::vec3(-6.0f,  0.0f,  -6.0f),
    glm::vec3(6.0f,  0.0f, 7.0f),
    glm::vec3(-4.0f,  0.0f, 6.0f),
    };

    // directional light
    glUniform3f(glGetUniformLocation(lightingShader, "dirLight.direction"), -0.2f, -1.0f, -0.3f);  //TODO mijenjace poziciju zavisno od doba dana
    glUniform3f(glGetUniformLocation(lightingShader, "dirLight.ambient"), 0.05f, 0.05f, 0.05f);
    glUniform3f(glGetUniformLocation(lightingShader, "dirLight.diffuse"), 0.4f, 0.4f, 0.4f);
    glUniform3f(glGetUniformLocation(lightingShader, "dirLight.specular"), 0.5f, 0.5f, 0.5f);
    // point light 1
    glUniform3f(glGetUniformLocation(lightingShader, "pointLights[0].position"), pyramidPositions[0].x, pyramidPositions[0].y + 2.0f, pyramidPositions[0].z);
    glUniform3f(glGetUniformLocation(lightingShader, "pointLights[0].ambient"), 0.05f, 0.05f, 0.05f);
    glUniform3f(glGetUniformLocation(lightingShader, "pointLights[0].diffuse"), 0.8f, 0.8f, 0.8f);
    glUniform3f(glGetUniformLocation(lightingShader, "pointLights[0].specular"), 1.0f, 1.0f, 1.0f);
    glUniform1f(glGetUniformLocation(lightingShader, "pointLights[0].constant"), 1.0f);
    glUniform1f(glGetUniformLocation(lightingShader, "pointLights[0].linear"), 0.09f);
    glUniform1f(glGetUniformLocation(lightingShader, "pointLights[0].quadratic"), 0.032f);
    // point light 2
    glUniform3f(glGetUniformLocation(lightingShader, "pointLights[1].position"), pyramidPositions[1].x, pyramidPositions[1].y + 2.0f, pyramidPositions[1].z);
    glUniform3f(glGetUniformLocation(lightingShader, "pointLights[1].ambient"), 0.05f, 0.05f, 0.05f);
    glUniform3f(glGetUniformLocation(lightingShader, "pointLights[1].diffuse"), 0.8f, 0.8f, 0.8f);
    glUniform3f(glGetUniformLocation(lightingShader, "pointLights[1].specular"), 1.0f, 1.0f, 1.0f);
    glUniform1f(glGetUniformLocation(lightingShader, "pointLights[1].constant"), 1.0f);
    glUniform1f(glGetUniformLocation(lightingShader, "pointLights[1].linear"), 0.09f);
    glUniform1f(glGetUniformLocation(lightingShader, "pointLights[1].quadratic"), 0.032f);
    // point light 3
    glUniform3f(glGetUniformLocation(lightingShader, "pointLights[2].position"), pyramidPositions[2].x, pyramidPositions[2].y + 4.0f, pyramidPositions[2].z);
    glUniform3f(glGetUniformLocation(lightingShader, "pointLights[2].ambient"), 0.05f, 0.05f, 0.05f);
    glUniform3f(glGetUniformLocation(lightingShader, "pointLights[2].diffuse"), 0.8f, 0.8f, 0.8f);
    glUniform3f(glGetUniformLocation(lightingShader, "pointLights[2].specular"), 1.0f, 1.0f, 1.0f);
    glUniform1f(glGetUniformLocation(lightingShader, "pointLights[2].constant"), 1.0f);
    glUniform1f(glGetUniformLocation(lightingShader, "pointLights[2].linear"), 0.09f);
    glUniform1f(glGetUniformLocation(lightingShader, "pointLights[2].quadratic"), 0.032f);

    glUniform3f(glGetUniformLocation(lightingShader, "viewPos"), cameraTranslation.x, cameraTranslation.y, cameraTranslation.z); //todo adjust with view
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
    createFloor();

    // Compile shaders
    unsigned int phongShader = createShader("phong.vert", "phong.frag");
    unsigned int gouraudShader = createShader("gouraud.vert", "gouraud.frag");
    unsigned int activeShader = phongShader;

    // Create MVP matrices
    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    glm::mat4 projectionP = glm::perspective(glm::radians(45.0f), (float)mode->width / (float)mode->height, 0.1f, 100.0f);
    glm::mat4 projectionO = glm::ortho(-30.0f, 30.0f, -20.0f, 20.0f, 0.1f, 100.0f);
    glm::mat4 projection = projectionP;
    // Create the camera view matrix based on the biggest pyramid position
    glm::vec3 pyramidPosition(-4.0f, 0.0f, 6.0f);
    glm::vec3 cameraPosition = pyramidPosition + glm::vec3(0.0f, 30.0f, 0.0f);  
    glm::vec3 cameraFront = pyramidPosition - cameraPosition;
    glm::vec3 cameraUp = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::mat4 view = glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);

    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    glClearColor(0.337, 0.451, 0.51, 1.0);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Input actions
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
        else if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
        {
            projection = projectionP;
        }
        else if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
        {
            projection = projectionO;
        }
        if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
        {
            activeShader = phongShader;
        }
        else if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
        {
            activeShader = gouraudShader;
        }

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            cameraPosition += cameraSpeed * cameraUp;
        }
        else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            cameraPosition -= cameraSpeed * cameraUp;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            cameraPosition -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            cameraPosition += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {  //rotation
            float rotationAngle = glm::radians(cameraSpeed);
            cameraUp = glm::normalize(glm::vec3(glm::rotate(glm::mat4(1.0f), rotationAngle, cameraFront) * glm::vec4(cameraUp, 0.0f)));
        }
        else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            float rotationAngle = glm::radians(-cameraSpeed);
            cameraUp = glm::normalize(glm::vec3(glm::rotate(glm::mat4(1.0f), rotationAngle, cameraFront) * glm::vec4(cameraUp, 0.0f)));
        }
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) { //zoom
            cameraPosition += cameraSpeed * cameraFront / 20.0f;
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            cameraPosition -= cameraSpeed * cameraFront / 20.0f;
        }

        view = glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);
        glUseProgram(activeShader);
        setLight(activeShader, cameraPosition);
        glUseProgram(0);

        renderPyramids(activeShader, view, projection);
        renderFloor(activeShader, view, projection);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteProgram(phongShader);
    glDeleteProgram(gouraudShader);
    DeleteDesertVariables();

    glfwTerminate();
    return 0;
}
