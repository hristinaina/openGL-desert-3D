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
GLuint grassVAO, grassVBO;
unsigned waterTexture, grassTexture;


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


void createGrass(unsigned int texture) {
    // Vertices for the floor
    GLfloat vertices[] = {
        // Position            // Normal           // Texture coordinates
        0.0f, 3.0f, 0.0f,      1.0f, 0.0f, 1.0f,   0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,      1.0f, 0.0f, 1.0f,   0.0f, 1.0f,
        5.0f, 3.0f, 0.0f,      1.0f, 0.0f, 1.0f,   1.0f, 0.0f,
        5.0f, 0.0f, 0.0f,      1.0f, 0.0f, 1.0f,   1.0f, 1.0f,
    };

    glGenVertexArrays(1, &grassVAO);
    glGenBuffers(1, &grassVBO);

    glBindVertexArray(grassVAO);

    glBindBuffer(GL_ARRAY_BUFFER, grassVBO);
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
    grassTexture = texture;
    glBindTexture(GL_TEXTURE_2D, grassTexture); //to set up the texture
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);   //GL_NEAREST, GL_LINEAR
    glBindTexture(GL_TEXTURE_2D, 0);
}


void renderBush(unsigned int shaderProgram, glm::mat4 defaltModel, glm::mat4 view, glm::mat4 projection) {
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

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

    // bind diffuse map
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, grassTexture);
    // bind specular map
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, grassTexture);

    //first grass
    glm::mat4 model = defaltModel;
    glUniformMatrix4fv(Mloc, 1, GL_FALSE, glm::value_ptr(model));
    glBindVertexArray(grassVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    //second grass
    model = glm::translate(model, glm::vec3(2.0f, 0.0f, 2.0f));
    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glUniformMatrix4fv(Mloc, 1, GL_FALSE, glm::value_ptr(model));
    glBindVertexArray(grassVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    //third grass
    model = defaltModel;
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 2.0f));
    model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glUniformMatrix4fv(Mloc, 1, GL_FALSE, glm::value_ptr(model));
    glBindVertexArray(grassVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    //fourth grass
    model = defaltModel;
    model = glm::translate(model, glm::vec3(0.3f, 0.0f, -2.0f));
    model = glm::rotate(model, glm::radians(-45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glUniformMatrix4fv(Mloc, 1, GL_FALSE, glm::value_ptr(model));
    glBindVertexArray(grassVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
}

void renderWater(unsigned int shaderProgram, glm::mat4 view, glm::mat4 projection) {

    glUseProgram(shaderProgram);

    //material
    glUniform1i(glGetUniformLocation(shaderProgram, "material.diffuse"), 0);
    glUniform1i(glGetUniformLocation(shaderProgram, "material.specular"), 0);
    glUniform1f(glGetUniformLocation(shaderProgram, "map"), 0);
    glUniform1f(glGetUniformLocation(shaderProgram, "material.shininess"), 32.0f);
    glUniform1f(glGetUniformLocation(shaderProgram, "alpha"), 0.5f);

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
