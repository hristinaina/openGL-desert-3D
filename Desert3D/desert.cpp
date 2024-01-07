#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <sstream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "desert.h"
#include "helper.h"
#include <glm/gtc/type_ptr.hpp>

GLuint pyramidVAO, pyramidVBO;

glm::vec3 pyramidPositions[] = {
    glm::vec3(0.0f,  0.0f,  0.0f),
    glm::vec3(2.0f,  5.0f, -15.0f),
    glm::vec3(0.3f, 0.3f, 0.3f)
};

void createPyramids() {

    GLfloat vertices[] = {
        // Base
        -1.0f, 0.0f, -1.0f,
        1.0f, 0.0f, -1.0f,
        0.0f, 0.0f, 1.0f,

        // Front face
        0.0f, 2.0f, 0.0f,
        -1.0f, 0.0f, -1.0f,
        1.0f, 0.0f, -1.0f,

        // Left face
        0.0f, 2.0f, 0.0f,
        -1.0f, 0.0f, -1.0f,
        0.0f, 0.0f, 1.0f,

        // Right face
        0.0f, 2.0f, 0.0f,
        0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, -1.0f
    };

    glGenVertexArrays(1, &pyramidVAO);
    glGenBuffers(1, &pyramidVBO);

    glBindVertexArray(pyramidVAO);

    glBindBuffer(GL_ARRAY_BUFFER, pyramidVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void renderPyramids(unsigned int shaderProgram, glm::mat4 VP) {
    glUseProgram(shaderProgram);

    GLint MVPloc = glGetUniformLocation(shaderProgram, "uMVP");
    int colorLoc = glGetUniformLocation(shaderProgram, "color");

    for (unsigned int i = 0; i < 3; i++)
    {
        // define the model matrix
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, pyramidPositions[i]);

        glUniformMatrix4fv(MVPloc, 1, GL_FALSE, glm::value_ptr(model * VP));
        glUniform4f(colorLoc, 0.85, 0.737, 0.204, 1.0);

        glBindVertexArray(pyramidVAO);

        glDrawArrays(GL_TRIANGLES, 0, 12);
    }

    glBindVertexArray(0);

    glUseProgram(0);
}

void DeleteDesertVariables() {
    glDeleteVertexArrays(1, &pyramidVAO);
}