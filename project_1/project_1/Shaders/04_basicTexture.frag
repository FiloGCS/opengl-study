#version 330 core

in vec3 vertexColor;
in vec2 texCoord;
uniform sampler2D u_texture1;
out vec4 FragColor;

void main(){
	FragColor = vec4(texCoord.x,texCoord.y,0.0f,0.0f);
	FragColor = texture(u_texture1, texCoord);
}