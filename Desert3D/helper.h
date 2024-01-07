#pragma once
#include <GL/glew.h>

unsigned loadImageToTexture(const char* filePath); //Ucitavanje teksture, izdvojeno u funkciju
unsigned int compileShader(GLenum type, const char* source);
unsigned int createShader(const char* vsSource, const char* fsSource);