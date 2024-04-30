// License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.
// https://www.shadertoy.com/view/MdBGDK
// By David Hoskins.
// Modified by Maria Messaoud-Nacer

#version 330

uniform float u_time;
uniform vec2 u_resolution;
uniform vec2 u_mouse;
uniform float amplitudeSon;

/* sortie du frament shader : une couleur */
out vec4 fragColor;
in vec2 vsoPosition;



void main(void) {
    float T = u_time, f = 3., g = f, d;
    vec2 r = vec2(1280, 720);
    vec2 m = r.xy;
    vec2 p;
    vec2 u = vsoPosition.xy;
    m = vec2(sin(T * .3) * sin(T * .17) * 1. + sin(T * .3), (1.0 - cos(T * .632)) * sin(T * .131) * 1.0 + cos(T * .3));
    m = (m + 1.0) * r;

    p = (2. + m - r) / r.y;

    for(int i = 0; i < 20; i++) {
        u = vec2(u.x, -u.y) / dot(u, u) + p, u.x = abs(u.x), f = max(f, dot(u - p, u - p)), g = min(g, sin(dot(u + p, u + p)) + 1.);
    }

    f = abs(-log(f) / 3.5);
    g = abs(-log(g) / 8.);

    fragColor = min(vec4(g, g * f, f, 0), 1.);
}
