#version 330 core

in vec2 TexCoord;
out vec4 FragColor;
uniform sampler2D texture1;

void main() {
    //flips texture
    vec2 flippedTexCoords = vec2(TexCoord.x, 1.0 - TexCoord.y);
    FragColor = texture(texture1, flippedTexCoords);
}