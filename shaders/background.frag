#version 110

uniform vec2 u_resolution;

varying vec3 color;
varying vec2 pos;

const float line = 0.02;
void main()
{
    vec2 uv = 2.0 * (gl_FragCoord.xy / u_resolution) - vec2(1.0);
    uv.x *= u_resolution.x / u_resolution.y;
    uv = pos;

    vec3 col = color;


    if(abs(uv.x) < line / 2.0 || abs(uv.y) < line / 2.0)
    {
      col = 1.0 - color;
    } else if(fract(uv.x * 5.0) < line || fract(uv.y * 5.0) < line)
    {
      col = 1.0 - color;
    }

    gl_FragColor = vec4(col ,1.0);
};
