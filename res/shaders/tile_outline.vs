#version 330

in vec3 aPos;

void main() {
    gl_Position = vec4(aPos, 1.0);
}