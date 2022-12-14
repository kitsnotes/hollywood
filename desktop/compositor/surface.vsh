#version 440

layout(location = 0) in vec2 qt_VertexPosition;
layout(location = 1) in vec2 qt_VertexTexCoord;
layout(location = 0) out vec2 v_texcoord;

layout(std140, binding = 0) uniform buf {
    mat4 qt_Matrix;
    float qt_Opacity;
}

void main()
{
    gl_Position = qt_Matrix * vec4(qt_VertexPosition, 0,0 ,1.0);
    v_texcoord = qt_VertexTextCoord;
}
