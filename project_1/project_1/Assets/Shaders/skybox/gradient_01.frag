#version 330 core
out vec4 FragColor;
  
in vec2 TexCoords;
//Engine
uniform vec2 resolution;
uniform float time;

uniform sampler2D screenTexture;

const float PHI = 1.61803398874989484820459; // Golden ratio
float gold_noise(in vec2 xy, in float seed){
       return fract(tan(distance(xy*PHI, xy)*seed)*xy.x);
}

void main()
{ 
    float lambda = clamp(gl_FragCoord.y*1.5f/resolution.y,0,1);
    float vignette = distance(resolution/2,gl_FragCoord.xy)/resolution.x;
    vignette = 1-clamp(vignette,0,1);
    lambda = lambda * vignette;
    float noise = 0.08f * gold_noise(gl_FragCoord.xy,1)-0.04f;

    vec3 color0 = vec3(1,1,1) * (0.15f) + noise*0.1f;
    vec3 color1 = vec3(1,1,1) * (0.35f) + noise*0.1f;
    vec3 color = color1*lambda + color0 * (1-lambda);
    FragColor = vec4(color,1);
}