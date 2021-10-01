#version 330 core
out vec4 FragColor;

in vec3 texColor;

void main() {
    FragColor = vec4(texColor, 1.0);
}