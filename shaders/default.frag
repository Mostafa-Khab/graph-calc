#version 110
varying vec3 color;
uniform float u_alpha;
void main()
{
    gl_FragColor = vec4(color , u_alpha);
};
