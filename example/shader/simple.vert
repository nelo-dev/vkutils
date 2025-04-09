#version 450

// Vertex input (position and UV)
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in float inTexIndex;

// Vertex output (to fragment shader)
layout(location = 0) out vec3 fragTexCoord;

// UBO for transformation matrices
layout(binding = 1) uniform UBO {
    mat4 model;
    mat4 view;
    mat4 projection;
} ubo;

void main() {
    // Pass texture coordinates to the fragment shader
    fragTexCoord = vec3(inTexCoord, inTexIndex);

    // Compute the final position using model-view-projection matrices
    gl_Position = ubo.projection * ubo.view * ubo.model * vec4(inPosition, 1.0);
}