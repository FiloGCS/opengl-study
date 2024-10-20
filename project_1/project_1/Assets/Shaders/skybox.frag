#version 330 core
out vec4 FragColor;
  
in vec2 TexCoords;
//Engine
uniform vec2 resolution;
uniform float time;

uniform sampler2D screenTexture;

float rand(vec2 co){
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

void main()
{ 
    float lambda = gl_FragCoord.y/1000;
    vec3 color0 = vec3(0.8,0.4,1) * (0.12f + 0.02f*rand(gl_FragCoord.xy*0.11));
    vec3 color1 = vec3(0.8,0.6,1) * (0.25f + 0.02f*rand(gl_FragCoord.xy*0.12));
    vec3 color = color1*lambda + color0 * (1-lambda);
    FragColor = vec4(color,1);
}