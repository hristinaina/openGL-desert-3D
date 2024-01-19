#version 330 core

in vec3 FragColor;
in vec2 TextureCoords;
uniform sampler2D map;
uniform float alpha;

out vec4 Color;

void main()
{
    vec4 texture = texture(map, TextureCoords.xy);
    vec3 final = texture.rgb * FragColor;
    final = pow(final, vec3(1.0 / 2.2)); //Apply gamma correction to the final values.
    // Check if the alpha value of the texture at this pixel is greater than a threshold
    float alphaThreshold = 0.1;
    if (texture.a > alphaThreshold) {
        // There is texture at this pixel
        Color = vec4(final, alpha);
    } else {
        // No texture at this pixel, make it transparent
        Color = vec4(0.0, 0.0, 0.0, 0.0);
    }
}