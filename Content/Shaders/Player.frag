#version 330 core

uniform vec4 uColor;

in vec2 vTexCoord;

out vec4 fragColor;

void main() {
    fragColor = uColor;
}
