precision mediump float;
attribute vec2 inPos;
attribute vec2 aTexCoord;
varying vec2 vTexCoord;

attribute vec4 color;
varying vec4 fragColor;

void main(void)
{
    gl_Position = vec4(inPos, 0.0, 1.0);
    vTexCoord = inPos * 0.5 + 0.5;
}
