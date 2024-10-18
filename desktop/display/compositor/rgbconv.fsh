uniform sampler2D textureSampler;
varying highp vec2 uv;

void main() {
    gl_FragColor.rgb = 1.0*texture2D(textureSampler, uv).rgb;
    gl_FragColor.a = 1.0;
}
