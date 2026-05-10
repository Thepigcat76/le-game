#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D texture0;
uniform vec2 resolution;

out vec4 finalColor;

void main() {
    vec2 texelSize = 1.0 / resolution;
    vec4 texel = texture(texture0, fragTexCoord);
    float centerAlpha = texel.a;

    float maxDist = 3.0;
    float glow = 0.0;

    // Only calculate glow if this pixel is transparent
    if (centerAlpha < 0.1) {
        for (float x = -maxDist; x <= maxDist; x++) {
            for (float y = -maxDist; y <= maxDist; y++) {
                vec2 offset = vec2(x, y) * texelSize;
                float neighborAlpha = texture(texture0, fragTexCoord + offset).a;
                float dist = length(vec2(x, y));

                // Only consider neighbors that are not too far and are visible
                if (neighborAlpha > 0.1) {
                    glow += smoothstep(maxDist, 0.0, dist) * 0.15;
                }
            }
        }

        glow = clamp(glow, 0.0, 1.0);
    }

    vec3 glow_color = vec3(0.16, 0.13, 0.15); // outline texture

    finalColor = (centerAlpha > 0.1)
        ? texel
        : vec4(glow_color, glow);
}