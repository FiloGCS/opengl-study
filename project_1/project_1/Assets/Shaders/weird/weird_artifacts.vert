#version 330 core
//Built in GLSL variables for vertex shader
// https://www.khronos.org/opengl/wiki/Built-in_Variable_(GLSL) MIND THE LAST ")"
//in int gl_VertexID;
//in int gl_InstanceID;

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float time;

//Built in GLSL outputs for vertex shader
//vec4 gl_Position; - the clip-space output position of the current vertex.
//float gl_PointSize; - the pixel width/height of the point being rasterized. It only has a meaning when rendering point primitives.
out vec2 texCoord;
out vec3 normal;
out vec3 normal_world;
out vec3 position;

void main(){
	//texCoord = aTexCoord;
	//normal = aNormal;
	normal_world = mat3(model) * normal;
	
	position = aPos;
	gl_Position = projection * view * model * vec4(aPos,1);
}