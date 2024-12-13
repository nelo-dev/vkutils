#version 450 core

layout(location = 0) in vec2 texCoord;
layout(location = 0) out vec4 fragColor;

layout(binding = 0) uniform sampler2D screenTexture;

void main(void) {
    fragColor = texture(screenTexture, texCoord);
}