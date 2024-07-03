#version 110
uniform mat4 MVP;
uniform vec2 u_resolution;
attribute vec3 vCol;
attribute vec2 vPos;
varying vec3 color;
void main()
{
    gl_Position  = MVP * vec4(vPos.x / (u_resolution.x / u_resolution.y), vPos.y, 1.0, 1.0);
    gl_PointSize = 9.0;
    color = vCol;
}
