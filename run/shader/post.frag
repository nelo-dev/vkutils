#version 450 core

layout(binding = 0) uniform sampler2D colorImage; // The main rendered color image
layout(binding = 1) uniform sampler2D depthImage; // Depth image for effects

layout(location = 0) in vec2 texCoord; // Texture coordinates passed from the vertex shader
layout(location = 0) out vec4 outColor; // Final output color

void main(void) {
    // Fetch the color data
    vec3 baseColor = texture(colorImage, texCoord).rgb;

    // Chromatic aberration offsets
    float aberrationStrength = 0.01; // Adjust strength of effect
    vec2 redOffset = texCoord + vec2(-aberrationStrength, 0.0);
    vec2 greenOffset = texCoord;
    vec2 blueOffset = texCoord + vec2(aberrationStrength, 0.0);

    // Sample color channels with offsets
    float redChannel = texture(colorImage, redOffset).r;
    float greenChannel = texture(colorImage, greenOffset).g;
    float blueChannel = texture(colorImage, blueOffset).b;

    vec3 chromaticColor = vec3(redChannel, greenChannel, blueChannel);

    // Fetch depth value
    float depth = texture(depthImage, texCoord).r;

    // Vignette effect based on depth
    float vignetteStrength = 1.0 - smoothstep(0.0, 0.5, depth); // Adjust falloff
    float vignetteFactor = mix(1.0, 0.5, vignetteStrength); // Blend based on depth

    // Combine the effects
    vec3 finalColor = chromaticColor * vignetteFactor;

    // Output the final color with full alpha
    outColor = vec4(finalColor, 1.0);
}
