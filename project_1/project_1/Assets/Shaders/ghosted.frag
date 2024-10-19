#version 330 core
//Built in GLSL variables for fragment shaders
// https://www.khronos.org/opengl/wiki/Built-in_Variable_(GLSL) MIND THE LAST ")"
//in vec4 gl_FragCoord; //- Screen space coordinates

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
	//Dithering to grey out this entity
	if(int(gl_FragCoord.x + gl_FragCoord.y) % 3 != 0){
		discard;
	}

	vec3 lightdir = point1_position - position;
	lightdir = normalize(lightdir);
	float lambert = clamp(dot(normal_world, lightdir),0,1);
	float d = distance(position, point1_position);
	float intensity = point1_falloff/(point1_falloff+d);
	vec3 point1 = lambert * intensity * point1_color;
	
	vec4 lights = vec4(ambient_color,1);
	lights += vec4(point1,1);


	FragColor = texture(u_texture1, texCoord) * lights;
	FragColor = vec4(0.18,0.18,0.18,1) * lights;

	//FragColor = vec4(gl_FragCoord.x/resolution.x,gl_FragCoord.y/resolution.y,0,1);
}