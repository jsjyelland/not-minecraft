#version 330 core
layout (location = 0) in vec2 aPos;

out vec2 TexCoords;

uniform mat4 transform;

void main() {
    gl_Position = transform * vec4(aPos, 0.0, 1.0);
    TexCoords = aPos + vec2(0.5);
}  