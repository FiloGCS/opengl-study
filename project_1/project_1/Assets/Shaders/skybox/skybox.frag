#version 330 core
//IO
in vec3 textureDir;
out vec4 FragColor;

//Uniforms
uniform samplerCube cubemap;

void main()
{
	FragColor = texture(cubemap,textureDir);
}