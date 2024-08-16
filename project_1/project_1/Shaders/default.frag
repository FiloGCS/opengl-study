#version 330 core

in vec3 vertexColor;
in vec2 texCoord;
in vec3 normal;
in vec4 normal_world;
in vec3 position;
uniform sampler2D u_texture1;
uniform sampler2D u_texture2;
out vec4 FragColor;

uniform float time;

float rand(vec2 co){
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

void main(){
	//FragColor = mix(texture(u_texture1, texCoord),texture(u_texture2, texCoord),0f);
	//FragColor = vec4(normal_world.x, normal_world.y, normal_world.z, 1);
	//FragColor = texture(u_texture1, texCoord) * texture(u_texture2,texCoord);

	vec4 lightPosition = vec4(sin(time), cos(time), 0,1);
	//vec4 lightPosition = vec4(1,2,0,1);
	float a = dot(normal_world, lightPosition);
	float d = distance(position, lightPosition.xyz);
	float fallof = 5;
	float intensity = fallof/(fallof+d);
	a = a * clamp(intensity,0,1);
	a = clamp(a,0,1);

	vec4 c1 = vec4(0.3,0,1,1);
	vec4 c2 = vec4(0,1,0,1);
	float lambda = ((sin((time + position.z)*5)+1)/2);

	float count = 0.5;
	while(count < 1 && rand(vec2(position.x,position.y)*count)<0.9){
		count = count + 0.05;
	}
	//FragColor = vec4(count,count,count,1);
	FragColor = texture(u_texture1, texCoord) * a * (intensity+count); 
}