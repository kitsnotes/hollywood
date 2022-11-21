attribute highp vec3 vertexCoord;
attribute highp vec2 textureCoord;
varying highp vec2 uv;
uniform highp mat4 vertexTransform;
uniform highp mat3 textureTransform;

void main() {
    uv = (textureTransform * vec3(textureCoord,1.0)).xy;
    gl_Position = vertexTransform * vec4(vertexCoord,1.0);
}
