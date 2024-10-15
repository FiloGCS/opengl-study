#version 330 core

#pragma enable_d3d11_debug_symbols
#pragma skip_optimizations d3d11

in vec3 vertexColor;
in vec2 texCoord;
in vec3 pos;
uniform sampler2D u_texture1;
uniform sampler2D u_texture2;
out vec4 FragColor;

void main(){
	vec2 uv = texCoord;

	uv -= vec2(0.5,0.5);
	uv *= vec2(2,2);
	float x = 0.05;
	int n = 5;
	float r = 0.5;
	float acum = 0;
	for(int i=0; i<n;i++){
		vec2 offset = vec2(r*i/n,r*i/n);
		float d = distance(uv,vec2(0,0) + offset);
		acum += x / (x + d);
	}
	acum /= n;
	float c = acum;
	
	FragColor = vec4(c, c, c, 1);

}