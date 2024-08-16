#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float time;

out vec2 texCoord;
out vec3 normal;
out vec3 normal_world;
out vec3 position;

void main(){
	texCoord = aTexCoord;
	normal = aNormal;
	normal_world = (model * vec4(normal.x, normal.y, normal.z, 1)).xyz;
	
	position = aPos;
	gl_Position = projection * view * model * vec4(aPos,1);
}