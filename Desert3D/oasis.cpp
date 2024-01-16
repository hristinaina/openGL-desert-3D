#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <sstream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "oasis.h"
#include <glm/gtc/type_ptr.hpp>

GLuint waterVAO, waterVBO;
unsigned waterTexture;


void createWater(unsigned int texture) {
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
    waterTexture = texture;
    glBindTexture(GL_TEXTURE_2D, waterTexture); //to set up the texture
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);   //GL_NEAREST, GL_LINEAR
    glBindTexture(GL_TEXTURE_2D, 0);
}

void renderWater(unsigned int shaderProgram, glm::mat4 view, glm::mat4 projection) {
    glUseProgram(shaderProgram);

    //material
    glUniform1i(glGetUniformLocation(shaderProgram, "material.diffuse"), 0);
    glUniform1i(glGetUniformLocation(shaderProgram, "material.specular"), 0);
    glUniform1f(glGetUniformLocation(shaderProgram, "map"), 0);
    glUniform1f(glGetUniformLocation(shaderProgram, "material.shininess"), 32.0f);
    glUniform1f(glGetUniformLocation(shaderProgram, "alpha"), 0.3f);

    GLint Mloc = glGetUniformLocation(shaderProgram, "uM");
    GLint Vloc = glGetUniformLocation(shaderProgram, "uV");
    GLint Ploc = glGetUniformLocation(shaderProgram, "uP");

    glm::mat4 model = glm::mat4(1.0f);
    glUniformMatrix4fv(Mloc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(Vloc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(Ploc, 1, GL_FALSE, glm::value_ptr(projection));

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