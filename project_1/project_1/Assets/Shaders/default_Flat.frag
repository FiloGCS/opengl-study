#version 330 core
//Built in GLSL variables for fragment shaders
// https://www.khronos.org/opengl/wiki/Built-in_Variable_(GLSL) MIND THE LAST ")"
//in vec4 gl_FragCoord; - Screen space coordinates

in vec3 vertexColor;
in vec2 texCoord;
in vec3 normal;
in vec3 normal_world;
in vec3 position;

uniform sampler2D u_texture1;
uniform sampler2D u_texture2;
//Engine
uniform vec2 resolution;
uniform float time;
//Lighting
//TODO - Change this to use unform arrays?
uniform vec3 ambient_color;
uniform vec3 point1_position;
uniform vec3 point1_color;
uniform float point1_falloff;

out vec4 FragColor;


float rand(vec2 co){
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

void main(){
	float c = abs(dot(normal_world, vec3(0,0,1)));
	FragColor = vec4(c,c,c,1);
}