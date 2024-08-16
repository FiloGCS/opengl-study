#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec2 texCoord;
out vec3 normal;
out vec4 normal_world;
out vec3 position;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float time;

void main(){
	texCoord = aTexCoord;
	normal = aNormal;
	normal_world = vec4(normal.x, normal.y, normal.z, 1);
	normal_world = model * normal_world;

	vec4 pos = vec4(aPos, 1.0f);
	//pos = pos + vec4(vec3(1,0,0)*sin(time*5),1);
	gl_Position = projection * view * model * pos;
	position = gl_Position.xyz;
}