#ifndef DESERT_H
#define DESERT_H

void createPyramids(unsigned texture);

void createFloor(unsigned texture);

void renderPyramids(unsigned int shaderProgram, glm::mat4 view, glm::mat4 projection);

void renderFloor(unsigned int shaderProgram, glm::mat4 view, glm::mat4 projection);

void createSignature(unsigned texture);

void renderSignature(unsigned int shaderProgram, glm::mat4 view, glm::mat4 projection);

#endif