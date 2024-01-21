#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <sstream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "desert.h"
#include <glm/gtc/type_ptr.hpp>

GLuint pyramidVAO, pyramidVBO;
GLuint floorVAO, floorVBO;
GLuint signatureVAO, signatureVBO;
unsigned pyramidTexture, sandTexture, signatureTexture, blackTexture;


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


void createPyramids(unsigned texture, unsigned black) {

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
    pyramidTexture = texture;
    glBindTexture(GL_TEXTURE_2D, pyramidTexture); //to set up the texture
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);   //GL_NEAREST, GL_LINEAR
    glBindTexture(GL_TEXTURE_2D, 0);
    blackTexture = black;
}

void createFloor(unsigned texture, unsigned black) {
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
    sandTexture = texture;
    glBindTexture(GL_TEXTURE_2D, sandTexture); //to set up the texture
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);   //GL_NEAREST, GL_LINEAR
    glBindTexture(GL_TEXTURE_2D, 0);
}


void createSignature(unsigned texture) {
    float vertices[] =
    {   //X    Y            S    T    
        -20.0f, 0.0f,  24.0f, 0.0f, -1.0f, 0.0f,  1.0, -1.0,
        -48.0f, 0.0f,  24.0f,  0.0f, -1.0f, 0.0f,  -1.0, -1.0,
        -20.0f, 0.0f,  29.0f, 0.0f, -1.0f, 0.0f,  1.0, 1.0,
        -48.0f, 0.0f,  29.0f,  0.0f, -1.0f, 0.0f,  -1.0, 1.0,
    };

    glGenVertexArrays(1, &signatureVAO);
    glGenBuffers(1, &signatureVBO);

    glBindVertexArray(signatureVAO);

    glBindBuffer(GL_ARRAY_BUFFER, signatureVAO);
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
    signatureTexture = texture;
    glBindTexture(GL_TEXTURE_2D, signatureTexture); //to set up the texture
    glBindTexture(GL_TEXTURE_2D, 0);
}

void renderPyramids(unsigned int shaderProgram, glm::mat4 view, glm::mat4 projection) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(shaderProgram);

    //material
    glUniform1i(glGetUniformLocation(shaderProgram, "material.diffuse"), 0);
    glUniform1i(glGetUniformLocation(shaderProgram, "material.specular"), 1);
    glUniform1f(glGetUniformLocation(shaderProgram, "material.shininess"), 10.0f);
    glUniform1f(glGetUniformLocation(shaderProgram, "map"), 0);
    glUniform1f(glGetUniformLocation(shaderProgram, "alpha"), 1.0f);

    GLint Mloc = glGetUniformLocation(shaderProgram, "uM");
    GLint Vloc = glGetUniformLocation(shaderProgram, "uV");
    GLint Ploc = glGetUniformLocation(shaderProgram, "uP");
    glUniformMatrix4fv(Vloc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(Ploc, 1, GL_FALSE, glm::value_ptr(projection));

    // bind diffuse map
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, pyramidTexture);
    // bind specular map
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, blackTexture);

    for (unsigned int i = 0; i < 3; i++)
    {
        // define the model matrix
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, pyramidPositions[i]);
        model = glm::scale(model, pyramidScaling[i]);

        glUniformMatrix4fv(Mloc, 1, GL_FALSE, glm::value_ptr(model));

        glBindVertexArray(pyramidVAO);

        glDrawArrays(GL_TRIANGLES, 0, 12);
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

void renderFloor(unsigned int shaderProgram, glm::mat4 view, glm::mat4 projection) {
    glUseProgram(shaderProgram);

    //material
    glUniform1i(glGetUniformLocation(shaderProgram, "material.diffuse"), 0);
    glUniform1i(glGetUniformLocation(shaderProgram, "material.specular"), 1);
    glUniform1f(glGetUniformLocation(shaderProgram, "map"), 0);
    glUniform1f(glGetUniformLocation(shaderProgram, "material.shininess"), 10.0f);
    glUniform1f(glGetUniformLocation(shaderProgram, "alpha"), 1.0f);

    GLint Mloc = glGetUniformLocation(shaderProgram, "uM");
    GLint Vloc = glGetUniformLocation(shaderProgram, "uV");
    GLint Ploc = glGetUniformLocation(shaderProgram, "uP");

    glUniformMatrix4fv(Vloc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(Ploc, 1, GL_FALSE, glm::value_ptr(projection));

    // bind diffuse map
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sandTexture);
    // bind specular map
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, blackTexture);

    for (unsigned int i = 0; i < 3; i++)
    {
        // define the model matrix
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, floorPositions[i]);

        glUniformMatrix4fv(Mloc, 1, GL_FALSE, glm::value_ptr(model));

        glBindVertexArray(floorVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    // define the model matrix
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(10.0f, -2.0f, 0.0f));
    glUniformMatrix4fv(Mloc, 1, GL_FALSE, glm::value_ptr(model));
    glBindVertexArray(floorVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}


void renderSignature(unsigned int shaderProgram, glm::mat4 view, glm::mat4 projection) {

    glUseProgram(shaderProgram);

    glUniform1i(glGetUniformLocation(shaderProgram, "material.diffuse"), 0);
    glUniform1i(glGetUniformLocation(shaderProgram, "material.specular"), 0);
    glUniform1f(glGetUniformLocation(shaderProgram, "map"), 0);
    glUniform1f(glGetUniformLocation(shaderProgram, "material.shininess"), 32.0f);
    glUniform1f(glGetUniformLocation(shaderProgram, "alpha"), 1.0f);

    GLint Mloc = glGetUniformLocation(shaderProgram, "uM");
    GLint Vloc = glGetUniformLocation(shaderProgram, "uV");
    GLint Ploc = glGetUniformLocation(shaderProgram, "uP");

    glUniformMatrix4fv(Vloc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(Ploc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(Mloc, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));

    // bind diffuse map
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, signatureTexture);
    // bind specular map
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, signatureTexture);

    glBindVertexArray(signatureVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}