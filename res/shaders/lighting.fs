#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D texture0;
uniform vec2 lightPos;
uniform vec3 lightColor;
uniform float lightRadius;
uniform float ambientLight;

out vec4 finalColor;

void main() {
    vec4 texel = texture(texture0, fragTexCoord);

    float dist = distance(lightPos, fragTexCoord);
    float intensity = ambientLight + smoothstep(lightRadius, 0.0, dist);

    vec3 litColor = texel.rgb * (lightColor * intensity);

    finalColor = vec4(litColor, texel.a);
}
