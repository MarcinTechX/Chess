#version 120

uniform float maxDistance;      

void main() 
{
    vec2 coord = gl_TexCoord[0].xy;

    float dist = length(coord - vec2(0.5, 0.5)); 
    float alpha = 1.0 - (dist * dist * dist / (maxDistance * maxDistance * maxDistance)); 
    gl_FragColor = vec4(1.0, 0.0, 0.0, alpha); 
}

