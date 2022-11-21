varying highp vec2 uv;

uniform sampler2D textureSampler;

void main() {
    gl_FragColor.rgb = 1.0*texture2D(textureSampler, uv).rgb;
    gl_FragColor.a = 1.0;
}
