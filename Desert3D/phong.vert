#version 330 core

layout (location = 0) in vec3 aPos;
layout(location = 1) in vec3 inNor; 
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out vec3 chFragPos; //Interpolirana pozicija fragmenta
out vec3 chNor; //Interpolirane normale

uniform mat4 uM;
uniform mat4 uV;
uniform mat4 uP;

void main()
{
    gl_Position = uP * uV * uM * vec4(aPos, 1.0);

    chFragPos = vec3(uM * vec4(aPos, 1.0));
	chNor = mat3(transpose(inverse(uM))) * inNor; //Inverziju matrica bolje racunati na CPU
     TexCoords = aTexCoords;
}