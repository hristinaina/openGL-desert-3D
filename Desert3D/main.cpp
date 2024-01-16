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

using namespace std;


float rotationSpeed = 0.1;
float r = 0.8;
float initialTime = glfwGetTime();
float xLast, yLast;

GLuint pyramidVAO, pyramidVBO;
GLuint floorVAO, floorVBO;
unsigned pyramidTexture, sandTexture;

GLuint waterVAO, waterVBO;
unsigned waterTexture;

glm::vec3 pyramidPositions[] = {
    glm::vec3(-6.0f,  0.0f,  -6.0f),
    glm::vec3(6.0f,  0.0f, 7.0f),
    glm::vec3(-4.0f,  0.0f, 6.0f),
};

glm::vec3 pyramidScaling[] = {
    glm::vec3(2.0f,  1.2f,  2.0f),
    glm::vec3(2.3f,  1.2f, 2.3f),
    glm::vec3(3.5f,  2.2f, 3.5f),
};

glm::vec3 floorPositions[] = {
    glm::vec3(0.0f,  0.0f,  0.0f),
    glm::vec3(0.0f,  0.0f, 10.0f),
    glm::vec3(10.0f,  0.0f, 10.0f),
};


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

void setLight(Shader& lightingShader, glm::vec3 cameraTranslation, glm::vec3 pyramidPositions[], bool paused, bool restarted) {
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
    lightingShader.setVec3("dirLight.direction", xLast * 3, yLast * 5, xLast * 2);
    lightingShader.setVec3("dirLight.ambient", lightValue, lightValue * colorDelta, lightValue);
    lightingShader.setVec3("dirLight.diffuse", lightValue, lightValue * colorDelta, lightValue);
    lightingShader.setVec3("dirLight.specular", 0.0f, 0.0f, 0.0f);
    // point light 1
    lightingShader.setVec3("pointLights[0].position", pyramidPositions[0].x, pyramidPositions[0].y + 1.0f, pyramidPositions[0].z);
    lightingShader.setVec3("pointLights[0].ambient", 0.0f, 0.7f, 0.7f);
    lightingShader.setVec3("pointLights[0].diffuse", 0.6f, 0.6f, 0.6f);
    lightingShader.setVec3("pointLights[0].specular", 0.5f, 0.5f, 0.5f);
    lightingShader.setFloat("pointLights[0].constant", 1.0f);
    lightingShader.setFloat("pointLights[0].linear", 0.7f);
    lightingShader.setFloat("pointLights[0].quadratic", 1.8f);
    // point light 2                 
    lightingShader.setVec3("pointLights[1].position", pyramidPositions[1].x, pyramidPositions[1].y + 1.0f, pyramidPositions[1].z);
    lightingShader.setVec3("pointLights[1].ambient", 0.0f, 0.7f, 0.7f);
    lightingShader.setVec3("pointLights[1].diffuse", 0.6f, 0.6f, 0.6f);
    lightingShader.setVec3("pointLights[1].specular", 0.5f, 0.5f, 0.5f);
    lightingShader.setFloat("pointLights[1].constant", 1.0f);
    lightingShader.setFloat("pointLights[1].linear", 0.7f);
    lightingShader.setFloat("pointLights[1].quadratic", 1.8f);
    // point light 3
    lightingShader.setVec3("pointLights[2].position", pyramidPositions[2].x, pyramidPositions[2].y + 1.0f, pyramidPositions[2].z);
    lightingShader.setVec3("pointLights[2].ambient", 0.0f, 0.7f, 0.7f);
    lightingShader.setVec3("pointLights[2].diffuse", 0.6f, 0.6f, 0.6f);
    lightingShader.setVec3("pointLights[2].specular", 0.5f, 0.5f, 0.5f);
    lightingShader.setFloat("pointLights[2].constant", 1.0f);
    lightingShader.setFloat("pointLights[2].linear", 0.7f);
    lightingShader.setFloat("pointLights[2].quadratic", 1.8f);

    lightingShader.setVec3("viewPos", cameraTranslation.x, cameraTranslation.y, cameraTranslation.z); //todo adjust with view

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

void renderSphere(Shader shaderProgram, glm::mat4 view, glm::mat4 projection, Model sphere, glm::vec3 pyramidPositions[]) {
    shaderProgram.use();

    //material
    shaderProgram.setInt("material.diffuse", 0);
    shaderProgram.setInt("material.specular", 0);
    shaderProgram.setFloat("map", 0);
    shaderProgram.setFloat("material.shininess", 32.0f);
    shaderProgram.setFloat("alpha", 1.0f);

    shaderProgram.setMat4("uV", view);
    shaderProgram.setMat4("uP", projection);

    for (unsigned int i = 0; i < 3; i++)
    {
        // define the model matrix
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, pyramidPositions[i] + glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));

        shaderProgram.setMat4("uM", model);

        sphere.Draw(shaderProgram);
    }

    glUseProgram(0);
}


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

void createPyramids() {

    GLfloat vertices[] = {

        // Back face
        -1.0f, 0.0f, -1.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
        0.0f, 3.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.5f, 1.0f,
         1.0f, 0.0f, -1.0f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f,

         // Front face
        0.0f, 3.0f, 0.0f,  0.0f, 0.0f, -1.0f,    0.5f, 1.0f,
       -1.0f, 0.0f, 1.0f,  0.0f, 0.0f, -1.0f,   0.0f, 0.0f,
        1.0f, 0.0f, 1.0f,  0.0f, 0.0f, -1.0f,   1.0f, 0.0f,

        // Left face
         0.0f, 3.0f, 0.0f,  1.0f, 0.0f, 0.0f,    0.5f, 1.0f,
        -1.0f, 0.0f, -1.0f,  1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
         -1.0f, 0.0f,  1.0f,  1.0f, 0.0f, 0.0f,   1.0f, 0.0f,

         // Right face
          0.0f, 3.0f, 0.0f,  -1.0f, 0.0f, 0.0f,     0.5f, 1.0f,
          1.0f, 0.0f,  1.0f,  -1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
          1.0f, 0.0f, -1.0f,  -1.0f, 0.0f, 0.0f,   1.0f, 0.0f,
    };

    glGenVertexArrays(1, &pyramidVAO);
    glGenBuffers(1, &pyramidVBO);

    glBindVertexArray(pyramidVAO);

    glBindBuffer(GL_ARRAY_BUFFER, pyramidVBO);
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
    pyramidTexture = TextureFromFile("pyramid-texture.jpg", "res");
    glBindTexture(GL_TEXTURE_2D, pyramidTexture); //to set up the texture
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);   //GL_NEAREST, GL_LINEAR
    glBindTexture(GL_TEXTURE_2D, 0);
}

void createFloor() {
    // Vertices for the floor
    GLfloat vertices[] = {
        -10.0f, 0.0f, -10.0f, 0.0f, -1.0f, 0.0f,  -1.0, -1.0,
        -10.0f, 0.0f,  0.0f, 0.0f, -1.0f, 0.0f,  -1.0, 1.0,
         0.0f, 0.0f, -10.0f, 0.0f, -1.0f, 0.0f,  1.0, -1.0,
         0.0f, 0.0f,  0.0f, 0.0f, -1.0f, 0.0f,  1.0, 1.0,
    };

    glGenVertexArrays(1, &floorVAO);
    glGenBuffers(1, &floorVBO);

    glBindVertexArray(floorVAO);

    glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
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
    sandTexture = TextureFromFile("sand.jpg", "res");
    glBindTexture(GL_TEXTURE_2D, sandTexture); //to set up the texture
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);   //GL_NEAREST, GL_LINEAR
    glBindTexture(GL_TEXTURE_2D, 0);
}

void renderPyramids(Shader shaderProgram, glm::mat4 view, glm::mat4 projection) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    shaderProgram.use();

    //material
    shaderProgram.setInt("material.diffuse", 0);
    shaderProgram.setInt("material.specular", 0);
    shaderProgram.setFloat("map", 0);
    shaderProgram.setFloat("material.shininess", 32.0f);
    shaderProgram.setFloat("alpha", 1.0f);

    shaderProgram.setMat4("uV", view);
    shaderProgram.setMat4("uP", projection);

    // bind diffuse map
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, pyramidTexture);
    // bind specular map
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, pyramidTexture);

    for (unsigned int i = 0; i < 3; i++)
    {
        // define the model matrix
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, pyramidPositions[i]);
        model = glm::scale(model, pyramidScaling[i]);

        shaderProgram.setMat4("uM", model);

        glBindVertexArray(pyramidVAO);

        glDrawArrays(GL_TRIANGLES, 0, 12);
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

void renderFloor(Shader shaderProgram, glm::mat4 view, glm::mat4 projection) {
    shaderProgram.use();

    //material
    shaderProgram.setInt("material.diffuse", 0);
    shaderProgram.setInt("material.specular", 0);
    shaderProgram.setFloat("map", 0);
    shaderProgram.setFloat("material.shininess", 32.0f);
    shaderProgram.setFloat("alpha", 1.0f);

    shaderProgram.setMat4("uV", view);
    shaderProgram.setMat4("uP", projection);

    // bind diffuse map
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sandTexture);
    // bind specular map
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, sandTexture);

    for (unsigned int i = 0; i < 3; i++)
    {
        // define the model matrix
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, floorPositions[i]);

        shaderProgram.setMat4("uM", model);

        glBindVertexArray(floorVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);
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

    fish.Draw(shaderProgram);

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

int main() {
    GLFWwindow* window = initWindow();
    if (!window) return -1;

    // Create objects
    createWater();
    createPyramids();
    createFloor();
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
        setLight(activeShader, cameraPosition, pyramidPeakPositions, paused, restared);
        glUseProgram(0);

        // render created objects
        renderWater(activeShader, view, projection);
        renderPyramids(activeShader, view, projection);
        renderFloor(activeShader, view, projection);
        renderSphere(activeShader, view, projection, sphere, pyramidPeakPositions);
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
