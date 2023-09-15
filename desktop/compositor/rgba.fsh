varying highp vec2 uv;
uniform sampler2D textureSampler;
void main() {
    highp vec4 tmp = texture2D(textureSampler, uv);
    tmp.a *= 1.0;
    gl_FragColor = tmp;
}
