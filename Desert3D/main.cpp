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
#include "shader.hpp"
#include "light.h"
#include "desert.h"
#include "oasis.h"

using namespace std;

bool paused = false;
bool restared = false;
float cameraSpeed = 0.03f;
float fishX = 0.0f;  // Initial X position of the fish
float fishSpeed = 0.2f;  // Speed of the fish movement
float fishAlpha = 1.0f;
float spotlightIntensity = 0.5;
float intensityRate = 0.005;
bool renderGrass = true;

glm::vec3 pyramidPeakPositions[] = {
glm::vec3(-6.0f,  3.6f,  -6.0f),
glm::vec3(6.0f,  3.6f, 7.0f),
glm::vec3(-4.0f, 6.6f, 6.0f),
};

void renderFish(Shader shaderProgram, glm::mat4 view, glm::mat4 projection, Model fish, float fishAlpha) {
    shaderProgram.use();

    //material
    shaderProgram.setInt("material.diffuse", 0);
    shaderProgram.setInt("material.specular", 0);
    shaderProgram.setFloat("map", 0);
    shaderProgram.setFloat("material.shininess", 32.0f);
    shaderProgram.setFloat("alpha", fishAlpha);

    shaderProgram.setMat4("uV", view);
    shaderProgram.setMat4("uP", projection);

    // define the model matrix
    glm::mat4 model = glm::mat4(1.0f);
    fishX = sin(glfwGetTime() * fishSpeed) *3; 
    model = glm::translate(model, glm::vec3(5.0f + fishX, -1.0f, -5.0f));
    model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
    model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 1.0f));

    shaderProgram.setMat4("uM", model);

    fish.Draw(shaderProgram.ID);

    glUseProgram(0);
}

void renderSphere(unsigned int shaderProgram, glm::mat4 view, glm::mat4 projection, Model sphere, Model cube, glm::vec3 pyramidPositions[]) {
    glUseProgram(shaderProgram);

    //material
    glUniform1i(glGetUniformLocation(shaderProgram, "material.diffuse"), 0);
    glUniform1i(glGetUniformLocation(shaderProgram, "material.specular"), 0);
    glUniform1f(glGetUniformLocation(shaderProgram, "material.shininess"), 32.0f);
    glUniform1f(glGetUniformLocation(shaderProgram, "map"), 0);
    glUniform1f(glGetUniformLocation(shaderProgram, "alpha"), 1.0f);

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

    // define the model matrix
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, pyramidPositions[2] + glm::vec3(0.0f, 2.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.002f, 0.002f, 0.002f));
    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    glUniformMatrix4fv(Mloc, 1, GL_FALSE, glm::value_ptr(model));
    cube.Draw(shaderProgram);

    glUseProgram(0);
}

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

int main() {
    GLFWwindow* window = initWindow();
    if (!window) return -1;

    //load textures
    unsigned pyramidTexture = TextureFromFile("pyramid-texture.jpg", "res");
    unsigned sandTexture = TextureFromFile("sand.jpg", "res");
    unsigned waterTexture = TextureFromFile("water.jpg", "res");
    unsigned grassTexture = TextureFromFile("grass.png", "res");
    unsigned signatureTexture = TextureFromFile("signature.png", "res");
    unsigned blackTexture = TextureFromFile("black.jpg", "res");

    // Create objects
    createWater(waterTexture);
    createPyramids(pyramidTexture, blackTexture);
    createFloor(sandTexture, blackTexture);
    createGrass(grassTexture);
    createSignature(signatureTexture);
    Model sphere(std::string("res/sphere.obj"));
    Model fish(std::string("res/Fish/12265_Fish_v1_L2.obj"));
    Model cube(std::string("res/Apple/10162_Apple_v01_l3.obj"));

    // Compile shaders
    Shader phongShader("phong.vert", "phong.frag");
    Shader gouraudShader("gouraud.vert", "gouraud.frag");
    Shader activeShader = phongShader;

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
    glm::vec3 cameraPosition2D = cameraPosition;
    glm::vec3 cameraFront2D = cameraFront;
    glm::vec3 cameraUp2D = cameraUp;
    glm::mat4 view = glm::lookAt(cameraPosition, pyramidPosition + glm::vec3(0.0f, 6.6f, 0.0f), cameraUp);
    glm::mat4 view2D = glm::lookAt(cameraPosition2D, pyramidPosition + glm::vec3(0.0f, 6.6f, 0.0f), cameraUp2D);

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
        // hide/show grass
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        {
            renderGrass = false;
            fishAlpha = 0.0;
        }
        else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        {
            renderGrass = true;
            fishAlpha = 1.0;
        }
        // choose shader type
        if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
        {
            activeShader = phongShader;
        }
        else if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
        {
            activeShader = gouraudShader;
        }
        // choose projection type
        if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
        {
            projection = projectionP;
        }
        else if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
        {
            projection = projectionO;
        }
        // pause or restart the scene
        if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
        {
            paused = true;
        }
        else if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        {
            restared = true;
            paused = false;
        }
        // decrease/increase spotlight intensity
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        {
            if (spotlightIntensity + intensityRate >= 1.0)
                spotlightIntensity = 1.0;
            else spotlightIntensity += intensityRate;
        }
        else if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        {
            if (spotlightIntensity - intensityRate <= 0.0)
                spotlightIntensity = 0.0;
            else spotlightIntensity -= intensityRate;
        }
        // move up-down
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        { 
            cameraPosition += cameraSpeed * cameraUp;
        }
        else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
            cameraPosition -= cameraSpeed * cameraUp;
        }
        // move left-right
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) 
        {
            cameraPosition -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        }
        else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) 
        {
            cameraPosition += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        }
        // rotate
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        {  
            float rotationAngle = glm::radians(cameraSpeed);
            cameraUp = glm::normalize(glm::vec3(glm::rotate(glm::mat4(1.0f), rotationAngle, cameraFront) * glm::vec4(cameraUp, 0.0f)));
        }
        else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        {
            float rotationAngle = glm::radians(-cameraSpeed);
            cameraUp = glm::normalize(glm::vec3(glm::rotate(glm::mat4(1.0f), rotationAngle, cameraFront) * glm::vec4(cameraUp, 0.0f)));
        }
        // zoom
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        { 
            cameraPosition += cameraSpeed * cameraFront / 20.0f;
        }
        else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        {
            cameraPosition -= cameraSpeed * cameraFront / 20.0f;
        }

        view = glm::lookAt(cameraPosition, pyramidPosition + glm::vec3(0.0f, 6.6f, 0.0f), cameraUp);
        activeShader.use();
        setLight(activeShader.ID, cameraPosition, pyramidPeakPositions, paused, restared, fishX, spotlightIntensity);
        glUseProgram(0);

        // render created objects 
        renderFish(activeShader, view, projection, fish, fishAlpha);
        renderWater(activeShader.ID, view, projection);
        renderFloor(activeShader.ID, view, projection);
        if (renderGrass) {
            renderBush(activeShader.ID, glm::translate(glm::mat4(1.f), glm::vec3(-0.5f, 0.0f, -5.0f)), view, projection);
            renderBush(activeShader.ID, glm::translate(glm::mat4(1.0f), glm::vec3(1.3f, 0.0f, -0.0f)), view, projection);
            renderBush(activeShader.ID, glm::translate(glm::mat4(1.0f), glm::vec3(0.1f, 0.0f, 0.0f)), view, projection);
            renderBush(activeShader.ID, glm::translate(glm::mat4(1.0f), glm::vec3(-0.2f, 0.0f, -0.9f)), view, projection);
            renderBush(activeShader.ID, glm::translate(glm::mat4(1.f), glm::vec3(8.0f, 0.0f, 0.2f)), view, projection);
        }
        renderPyramids(activeShader.ID, view, projection);   
        renderSphere(activeShader.ID, view, projection, sphere, cube, pyramidPeakPositions);
        renderSignature(activeShader.ID,  view2D, projectionO);
       
        glfwSwapBuffers(window);
        glfwPollEvents();

        restared = false;
    }

    glfwTerminate();
    return 0;
}
