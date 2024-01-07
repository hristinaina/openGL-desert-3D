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

GLuint pyramidsVAO[3], pyramidsVBO[3];

void createPyramid(int i) {

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

    glGenVertexArrays(1, &pyramidsVAO[i]);
    glGenBuffers(1, &pyramidsVBO[i]);

    glBindVertexArray(pyramidsVAO[i]);

    glBindBuffer(GL_ARRAY_BUFFER, pyramidsVBO[i]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void renderPyramid(unsigned int shaderProgram, glm::mat4 VP, int i) {
    glUseProgram(shaderProgram);

    GLint MVPloc = glGetUniformLocation(shaderProgram, "uMVP");
    int colorLoc = glGetUniformLocation(shaderProgram, "color");

    // define the model matrix
    glm::vec3 translation(i * 0.3f, 0.0f, i * 0.3f);
    glm::mat4 model = glm::translate(glm::mat4(1.0f), translation);

    glUniformMatrix4fv(MVPloc, 1, GL_FALSE, glm::value_ptr(model * VP));
    glUniform4f(colorLoc, 0.85, 0.737, 0.204, 1.0);

    glBindVertexArray(pyramidsVAO[i]);

    glDrawArrays(GL_TRIANGLES, 0, 12);

    glBindVertexArray(0);

    glUseProgram(0);
}

void DeleteDesertVariables() {
    glDeleteVertexArrays(1, &pyramidsVAO[0]);
    glDeleteVertexArrays(1, &pyramidsVAO[1]);
    glDeleteVertexArrays(1, &pyramidsVAO[2]);
}