#version 330 core
out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D screenTexture;

void main()
{ 
    FragColor = texture(screenTexture, TexCoords);
    FragColor = vec4(vec3(FragColor.x*0.2126+FragColor.y*0.7152+FragColor.z*0.0722),1);
}