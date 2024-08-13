#version 330 core

#pragma enable_d3d11_debug_symbols
#pragma skip_optimizations d3d11

layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoord;

out vec2 texCoord;
out vec3 pos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){
	//gl_Position = projection * view * model * vec4(aPos, 1.0f);
	gl_Position = projection * view * vec4(aPos, 1.0f);
	texCoord = aTexCoord;
}