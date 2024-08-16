#version 330 core

in vec3 vertexColor;
in vec2 texCoord;
in vec3 normal;
in vec3 normal_world;
in vec3 position;

uniform sampler2D u_texture1;
uniform sampler2D u_texture2;
//Lighting
uniform vec3 ambient_color;
uniform vec3 point1_position;
uniform vec3 point1_color;
uniform float point1_falloff;
uniform float time;

out vec4 FragColor;


float rand(vec2 co){
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

void main(){
	//FragColor = mix(texture(u_texture1, texCoord),texture(u_texture2, texCoord),0f);
	//FragColor = vec4(normal_world.x, normal_world.y, normal_world.z, 1);
	//FragColor = texture(u_texture1, texCoord) * texture(u_texture2,texCoord);
	vec3 lightdir = point1_position - position;
	lightdir = normalize(lightdir);
	float lambert = clamp(dot(normal_world, lightdir),0,1);
	float d = distance(position, point1_position);
	float intensity = point1_falloff/(point1_falloff+d);
	vec3 point1 = lambert * intensity * point1_color;
	
	vec4 lights = vec4(ambient_color,1);
	lights += vec4(point1,1);

	//FragColor = texture(u_texture1, texCoord) * vec4(point1,1);
	FragColor = vec4(1,1,1,1) * lights;
}