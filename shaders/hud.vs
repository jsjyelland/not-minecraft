#version 330 core
layout (location = 0) in vec2 aPos;

out vec2 TexCoords;

uniform mat4 transform;
uniform mat4 texTransform;

void main() {
    gl_Position = transform * vec4(aPos, 0.0, 1.0);
    TexCoords = (texTransform * vec4(aPos.x + 0.5, aPos.y + 0.5, 1.0, 1.0)).xy;
}  