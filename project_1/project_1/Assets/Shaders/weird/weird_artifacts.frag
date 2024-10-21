#version 330 core
//Built in GLSL variables for fragment shaders
// https://www.khronos.org/opengl/wiki/Built-in_Variable_(GLSL) MIND THE LAST ")"
//in vec4 gl_FragCoord; - Screen space coordinates

in vec3 vertexColor;
in vec2 texCoord;
in vec3 normal;
in vec3 normal_world;
in vec3 position;

out vec4 FragColor;

void main(){
	FragColor = vec4(normalize(normal_world.xyz),1);
}