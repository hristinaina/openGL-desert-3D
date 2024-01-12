#version 330 core

in vec3 FragColor;
in vec2 TextureCoords;
uniform sampler2D map;

out vec4 Color;

void main()
{
    vec4 texture = texture(map, TextureCoords.xy);
    vec3 final = texture.rgb * FragColor;
    final = pow(final, vec3(1.0 / 2.2)); //Apply gamma correction to the final values.
    Color = vec4(final, 1.0);
}