#version 450

// Fragment input (from vertex shader)
layout(location = 0) in vec3 fragTexCoord;

// Output color
layout(location = 0) out vec4 outColor;

// Texture sampler
layout(binding = 0) uniform sampler2DArray textureSampler;

void main() {
    // Sample the texture using the UV coordinates
    outColor = texture(textureSampler, fragTexCoord);
}