attribute vec2 position;
varying vec2 _uv;
void main() {
    gl_Position = vec4(position,0.0,1.0);
    vec2 uv = position * 0.5 + 0.5;
    _uv = vec2(uv.x, 1.0 - uv.y);
}
