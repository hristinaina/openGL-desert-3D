#pragma once

void createPyramids();

void renderPyramids(unsigned int shaderProgram, glm::mat4 view, glm::mat4 project);

void createFloor();

void renderFloor(unsigned int shaderProgram, glm::mat4 view, glm::mat4 project);

void DeleteDesertVariables();