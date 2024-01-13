#pragma once

void setLight(unsigned int lightingShader, glm::vec3 cameraTranslation, glm::vec3 pyramidPositions[], bool paused, bool restarted);

void renderSphere(unsigned int shaderProgram, glm::mat4 view, glm::mat4 projection, Model sphere, glm::vec3 pyramidPositions[]);