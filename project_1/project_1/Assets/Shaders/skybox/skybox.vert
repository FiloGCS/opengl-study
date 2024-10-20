#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    TexCoords = vec3(aPos.xy,-aPos.z);
    gl_Position = projection * view * vec4(aPos, 0.05);
}