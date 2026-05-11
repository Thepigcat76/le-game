#version 330

in vec2 fragTexCoord;

uniform sampler2D texture0;
uniform vec2 light_pos;
uniform vec3 light_color;
uniform float light_radius;
uniform float ambient_light;

out vec4 finalColor;

void main() {
    vec4 texColor = texture(texture0, fragTexCoord);

    float dist = distance(fragTexCoord, light_pos);
    float intensity = clamp(1.0 - dist / light_radius, 0.0, 1.0);

    vec3 lit = texColor.rgb * (ambient_light + intensity * light_color);
    finalColor = vec4(lit, texColor.a);
}