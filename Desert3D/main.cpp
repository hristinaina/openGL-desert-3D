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


using namespace std;

GLuint waterVAO, waterVBO;
unsigned waterTexture;

void createWater() {
    // Vertices for the floor
    GLfloat vertices[] = {
        0.0f, 0.0f, -10.0f,  0.0f, -1.0f, 0.0f,  -1.0, -1.0,
        0.0f, 0.0f,  0.0f,  0.0f, -1.0f, 0.0f,  -1.0, 1.0,
        10.0f, 0.0f, -10.0f, 0.0f, -1.0f, 0.0f,  1.0, -1.0,
        10.0f, 0.0f,  0.0f, 0.0f, -1.0f, 0.0f,  1.0, 1.0,
    };

    glGenVertexArrays(1, &waterVAO);
    glGenBuffers(1, &waterVBO);

    glBindVertexArray(waterVAO);

    glBindBuffer(GL_ARRAY_BUFFER, waterVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    // Texture coordinates attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //Texture
    waterTexture = TextureFromFile("water.jpg", "res");
    glBindTexture(GL_TEXTURE_2D, waterTexture); //to set up the texture
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);   //GL_NEAREST, GL_LINEAR
    glBindTexture(GL_TEXTURE_2D, 0);
}

void renderWater(Shader shaderProgram, glm::mat4 view, glm::mat4 projection) {
    shaderProgram.use();

    //material
    shaderProgram.setInt("material.diffuse", 0);
    shaderProgram.setInt("material.specular", 0);
    shaderProgram.setFloat("map", 0);
    shaderProgram.setFloat("material.shininess", 32.0f);
    shaderProgram.setFloat("alpha", 0.3f);

    glm::mat4 model = glm::mat4(1.0f);
    shaderProgram.setMat4("uM", model);
    shaderProgram.setMat4("uV", view);
    shaderProgram.setMat4("uP", projection);

    // bind diffuse map
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, waterTexture);
    // bind specular map
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, waterTexture);

    glBindVertexArray(waterVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}


void renderFish(Shader shaderProgram, glm::mat4 view, glm::mat4 projection, Model fish) {
    shaderProgram.use();

    //material
    shaderProgram.setInt("material.diffuse", 0);
    shaderProgram.setInt("material.specular", 0);
    shaderProgram.setFloat("map", 0);
    shaderProgram.setFloat("material.shininess", 32.0f);
    shaderProgram.setFloat("alpha", 1.0f);

    shaderProgram.setMat4("uV", view);
    shaderProgram.setMat4("uP", projection);

    // define the model matrix
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 4.0f, 0.0f));

    shaderProgram.setMat4("uM", model);

    fish.Draw(shaderProgram.ID);

    glUseProgram(0);
}


/*  TODO
* 9. dodati uniformu za odredjivanje providnosti ove boje tj teksture u fragment shader
* 10. dodati ime i br indeksa
*/

bool paused = false;
bool restared = false;
float cameraSpeed = 0.03f;

glm::vec3 pyramidPeakPositions[] = {
glm::vec3(-6.0f,  3.6f,  -6.0f),
glm::vec3(6.0f,  3.6f, 7.0f),
glm::vec3(-4.0f, 6.6f, 6.0f),
};

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


void renderSphere(unsigned int shaderProgram, glm::mat4 view, glm::mat4 projection, Model sphere, glm::vec3 pyramidPositions[]) {
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

    glUseProgram(0);
}

int main() {
    GLFWwindow* window = initWindow();
    if (!window) return -1;

    //load textures
    unsigned pyramidTexture = TextureFromFile("pyramid-texture.jpg", "res");
    unsigned sandTexture = TextureFromFile("sand.jpg", "res");

    // Create objects
    createWater();
    createPyramids(pyramidTexture);
    createFloor(sandTexture);
    Model sphere(std::string("res/sphere.obj"));
    Model fish(std::string("res/Fish/12265_Fish_v1_L2.obj"));

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
    glm::mat4 view = glm::lookAt(cameraPosition, pyramidPosition + glm::vec3(0.0f, 6.6f, 0.0f), cameraUp);

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
        // choose projection type
        else if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
        {
            projection = projectionP;
        }
        else if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
        {
            projection = projectionO;
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
        // move up-down
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) { 
            cameraPosition += cameraSpeed * cameraUp;
        }
        else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            cameraPosition -= cameraSpeed * cameraUp;
        }
        // move left-right
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)  
            cameraPosition -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            cameraPosition += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        // rotate
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {  
            float rotationAngle = glm::radians(cameraSpeed);
            cameraUp = glm::normalize(glm::vec3(glm::rotate(glm::mat4(1.0f), rotationAngle, cameraFront) * glm::vec4(cameraUp, 0.0f)));
        }
        else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            float rotationAngle = glm::radians(-cameraSpeed);
            cameraUp = glm::normalize(glm::vec3(glm::rotate(glm::mat4(1.0f), rotationAngle, cameraFront) * glm::vec4(cameraUp, 0.0f)));
        }
        // zoom
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) { 
            cameraPosition += cameraSpeed * cameraFront / 20.0f;
        }
        else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            cameraPosition -= cameraSpeed * cameraFront / 20.0f;
        }

        view = glm::lookAt(cameraPosition, pyramidPosition + glm::vec3(0.0f, 6.6f, 0.0f), cameraUp);
        activeShader.use();
        setLight(activeShader.ID, cameraPosition, pyramidPeakPositions, paused, restared);
        glUseProgram(0);

        // render created objects
        renderWater(activeShader, view, projection);
        renderPyramids(activeShader.ID, view, projection);
        renderFloor(activeShader.ID, view, projection);
        renderSphere(activeShader.ID, view, projection, sphere, pyramidPeakPositions);
        renderFish(activeShader, view, projection, fish);

        glfwSwapBuffers(window);
        glfwPollEvents();

        restared = false;
    }

    //glDeleteProgram(phongShader);
    //glDeleteProgram(gouraudShader);
    //DeleteDesertVariables();

    glfwTerminate();
    return 0;
}
