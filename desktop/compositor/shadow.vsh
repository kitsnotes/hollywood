/* attribute highp vec4 qt_Vertex;
attribute highp vec4 qt_MultiTexCoord0;
uniform highp mat4 qt_ModelViewProjectionMatrix;
varying highp vec4 qt_TexCoord0;*/
precision highp float;
uniform vec4 box;
uniform vec2 window;
uniform float sigma;
attribute vec2 coord;
varying vec2 vertex;
void main() {
    float padding = 3.0 * sigma;
    vertex = mix(box.xy - padding, box.zw + padding, coord);
    gl_Position = vec4(vertex / window * 2.0 - 1.0, 0.0, 1.0);
}
