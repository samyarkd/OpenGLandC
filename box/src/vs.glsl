#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec2 aTexCoord;

out vec3 linkedColorAttribute;
out vec2 TexCoord;

// 1x4 matrix
uniform mat4 transform;

void main()
{
    gl_Position = transform * vec4(aPos, 1.0);
    linkedColorAttribute = aColor;
    TexCoord = vec2(aTexCoord);
}
