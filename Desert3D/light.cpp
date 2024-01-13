#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <sstream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "model.hpp"
#include "light.h"

float rotationSpeed = 0.1;
float r = 0.8;
float initialTime = glfwGetTime();
float xLast, yLast;

void updateVariables(float paused, float restared) {
    float ydelta = r * (sin((glfwGetTime() - initialTime) * rotationSpeed));
    float xdelta = r * (cos((glfwGetTime() - initialTime) * rotationSpeed));
    if (!paused) {
        xLast = xdelta;
        yLast = ydelta;
    }
    if (restared) {
        initialTime = glfwGetTime();
    }
}

double mapRange(double value, double inMin, double inMax, double outMin, double outMax) {
    return outMin + (value - inMin) * (outMax - outMin) / (inMax - inMin);
}

void setLight(unsigned int lightingShader, glm::vec3 cameraTranslation, glm::vec3 pyramidPositions[], bool paused, bool restarted) {
    updateVariables(paused, restarted);
    glClearColor(0.243 + yLast / 2, 0.435 + yLast / 2, 0.529 + yLast / 2, 1.0);

    float lightValue = mapRange(yLast, -0.8, 0.8, 0.0, 0.6);
    float colorDelta;
    if (lightValue > 0.0f && lightValue < 0.45f) {
        colorDelta = mapRange(lightValue, 0.2, 0.45, 0.6, 1.0);;
    }
    else {
        colorDelta = 1.0;
    }

    // directional light
    glUniform3f(glGetUniformLocation(lightingShader, "dirLight.direction"), xLast*3, yLast*5, xLast*2); 
    glUniform3f(glGetUniformLocation(lightingShader, "dirLight.ambient"), lightValue, lightValue * colorDelta, lightValue);
    glUniform3f(glGetUniformLocation(lightingShader, "dirLight.diffuse"), lightValue, lightValue * colorDelta, lightValue);
    glUniform3f(glGetUniformLocation(lightingShader, "dirLight.specular"), 0.0f, 0.0f, 0.0f);
    // point light 1
    glUniform3f(glGetUniformLocation(lightingShader, "pointLights[0].position"), pyramidPositions[0].x, pyramidPositions[0].y + 1.0f, pyramidPositions[0].z);
    glUniform3f(glGetUniformLocation(lightingShader, "pointLights[0].ambient"), 0.0f, 0.7f, 0.7f);
    glUniform3f(glGetUniformLocation(lightingShader, "pointLights[0].diffuse"), 0.6f, 0.6f, 0.6f);
    glUniform3f(glGetUniformLocation(lightingShader, "pointLights[0].specular"), 0.5f, 0.5f, 0.5f);
    glUniform1f(glGetUniformLocation(lightingShader, "pointLights[0].constant"), 1.0f);
    glUniform1f(glGetUniformLocation(lightingShader, "pointLights[0].linear"), 0.7f);
    glUniform1f(glGetUniformLocation(lightingShader, "pointLights[0].quadratic"), 1.8f);
    // point light 2                 
    glUniform3f(glGetUniformLocation(lightingShader, "pointLights[1].position"), pyramidPositions[1].x, pyramidPositions[1].y + 1.0f, pyramidPositions[1].z);
    glUniform3f(glGetUniformLocation(lightingShader, "pointLights[1].ambient"), 0.0f, 0.7f, 0.7f);
    glUniform3f(glGetUniformLocation(lightingShader, "pointLights[1].diffuse"), 0.6f, 0.6f, 0.6f);
    glUniform3f(glGetUniformLocation(lightingShader, "pointLights[1].specular"), 0.5f, 0.5f, 0.5f);
    glUniform1f(glGetUniformLocation(lightingShader, "pointLights[1].constant"), 1.0f);
    glUniform1f(glGetUniformLocation(lightingShader, "pointLights[1].linear"), 0.7f);
    glUniform1f(glGetUniformLocation(lightingShader, "pointLights[1].quadratic"), 1.8f);
    // point light 3
    glUniform3f(glGetUniformLocation(lightingShader, "pointLights[2].position"), pyramidPositions[2].x, pyramidPositions[2].y + 1.0f, pyramidPositions[2].z);
    glUniform3f(glGetUniformLocation(lightingShader, "pointLights[2].ambient"), 0.0f, 0.7f, 0.7f);
    glUniform3f(glGetUniformLocation(lightingShader, "pointLights[2].diffuse"), 0.6f, 0.6f, 0.6f);
    glUniform3f(glGetUniformLocation(lightingShader, "pointLights[2].specular"), 0.5f, 0.5f, 0.5f);
    glUniform1f(glGetUniformLocation(lightingShader, "pointLights[2].constant"), 1.0f);
    glUniform1f(glGetUniformLocation(lightingShader, "pointLights[2].linear"), 0.7f);
    glUniform1f(glGetUniformLocation(lightingShader, "pointLights[2].quadratic"), 1.8f);

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

void renderSphere(unsigned int shaderProgram, glm::mat4 view, glm::mat4 projection, Model sphere, glm::vec3 pyramidPositions[]) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(shaderProgram);

    //material
    glUniform1i(glGetUniformLocation(shaderProgram, "material.diffuse"), 0);
    glUniform1i(glGetUniformLocation(shaderProgram, "material.specular"), 1);
    glUniform1f(glGetUniformLocation(shaderProgram, "material.shininess"), 32.0f);
    glUniform1f(glGetUniformLocation(shaderProgram, "map"), 0);

    GLint Mloc = glGetUniformLocation(shaderProgram, "uM");
    GLint Vloc = glGetUniformLocation(shaderProgram, "uV");
    GLint Ploc = glGetUniformLocation(shaderProgram, "uP");
    glUniformMatrix4fv(Vloc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(Ploc, 1, GL_FALSE, glm::value_ptr(projection));

    for (unsigned int i = 0; i < 3; i++)
    {
        // define the model matrix
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, pyramidPositions[i] + glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));

        glUniformMatrix4fv(Mloc, 1, GL_FALSE, glm::value_ptr(model));

        sphere.Draw(shaderProgram);
    }

    glUseProgram(0);
}