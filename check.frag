#version 120

#define M_PI 3.1415926535897932384626433832795

void main() 
{
    vec2 coord = gl_TexCoord[0].xy;

    float dist = length(coord - vec2(0.5, 0.5)); 
    float alpha = 1.5 * cos(dist * M_PI);
    gl_FragColor = vec4(1.0, 0.0, 0.0, alpha); 
}

