#version 330 core

out vec4 color;

in vec2 TexCoord;

uniform sampler2D ourTexture;

uniform float iTime;

void main() {
    vec4 texColor = texture(ourTexture, TexCoord);
    float red = sin(iTime);
    float green = sin(iTime + 2.0);
    float blue = sin(iTime + 4.0);
    color = vec4(red, green, blue, 1.0) * texColor;
}
