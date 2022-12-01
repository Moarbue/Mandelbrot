#version 410

precision highp float;

in vec2 uv;
out vec4 out_color;

uniform int iterations;
uniform int width;
uniform int height;
uniform float xmin;
uniform float xmax;
uniform float ymin;
uniform float ymax;

float mapf(float x, float in_min, float in_max, float out_min, float out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void main(void) {

    float x0 = mapf(gl_FragCoord.x, 0.5, width - 0.5, xmin, xmax);
    float y0 = mapf(gl_FragCoord.y, 0.5, height - 0.5, ymin, ymax);

    int i = -1;
    float x = 0.0;
    float y = 0.0;

    while ((x*x + y*y) <= 2*2 && ++i < iterations) {
        float xtmp = x*x - y*y + x0;
        y = 2 * x * y + y0;
        x = xtmp;
    }

    out_color = vec4(i * 1.0/iterations, 0.0, 0.0, 1.0);
}