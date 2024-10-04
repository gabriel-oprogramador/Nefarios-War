#version 330 core

uniform vec4 uColor;

in vec2 vTexCoord;

out vec4 fragColor;
vec4 finalColor;

void main() {
    finalColor= uColor;
    if(finalColor.a <= 0.2)
        discard;
    fragColor = uColor;
}
