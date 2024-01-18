#pragma once

void createWater(unsigned int texture);

void renderWater(unsigned int shaderProgram, glm::mat4 view, glm::mat4 projection);

void createGrass(unsigned int texture);

void renderBush(unsigned int shaderProgram, glm::mat4 model, glm::mat4 view, glm::mat4 projection);