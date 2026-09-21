#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture; // linear HDR scene color
uniform float uExposure = 1.0;

void main()
{
    vec3 hdr = texture(screenTexture, TexCoords).rgb;

    // Tone map (Reinhard): compress unbounded linear radiance into [0, 1)
    hdr *= uExposure;
    vec3 mapped = hdr / (hdr + vec3(1.0));

    // Gamma encode for the display. Inputs were decoded from sRGB on sample (GL_SRGB8 textures),
    // so this is the only place the curve is applied.
    mapped = pow(mapped, vec3(1.0 / 2.2));

    FragColor = vec4(mapped, 1.0);
}
