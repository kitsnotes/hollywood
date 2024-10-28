precision mediump float;
uniform vec4 fragColor;
uniform float radius;
varying vec2 vTexCoord;

void main()
{

/*    vec2 uv = vTexCoord * 2.0 - 1.0; // Convert back to NDC
    uv.x = abs(uv.x);
    uv.y = abs(uv.y);

    // Calculate distance to corners
    float cornerDist = length(max(uv - vec2(1.0, 1.0), 0.0));

    // If the distance is less than the radius, discard the fragment
    if (cornerDist > radius) {
        discard;
    } */

    gl_FragColor = fragColor;
}
