//#shader vertex
#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main() {
    TexCoords = aTexCoords;
    gl_Position = vec4(aPos, 0.0, 1.0);
}

//#shader fragment
#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

// 0 = no effect, 1 = inversion, 2 = grayscale, 3 = sharpen, 4 = blur, 5 = edge detection
uniform int effect;

const float offset = 1.0 / 300.0;

vec3 apply_kernel(float kernel[9]) {
    vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // top-left
        vec2( 0.0f,    offset), // top-center
        vec2( offset,  offset), // top-right
        vec2(-offset,  0.0f),   // center-left
        vec2( 0.0f,    0.0f),   // center-center
        vec2( offset,  0.0f),   // center-right
        vec2(-offset, -offset), // bottom-left
        vec2( 0.0f,   -offset), // bottom-center
        vec2( offset, -offset)  // bottom-right
    );

    vec3 sample_tex[9];
    for (int i = 0; i < 9; i++) {
        sample_tex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
    }

    vec3 color = vec3(0.0);
    for (int i = 0; i < 9; i++) {
        color += sample_tex[i] * kernel[i];
    }
    return color;
}

void main() {
    vec3 color = texture(screenTexture, TexCoords).rgb;

    if (effect == 1) {
        // invert
        color = vec3(1.0) - color;
    } else if (effect == 2) {
        // grayscale
        float average = 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
        color = vec3(average);
    } else if (effect == 3) {
        // sharp
        float kernel[9] = float[](
            -1, -1, -1,
            -1,  9, -1,
            -1, -1, -1
        );
        color = apply_kernel(kernel);
    } else if (effect == 4) {
        // brul
        float kernel[9] = float[](
            1.0 / 16, 2.0 / 16, 1.0 / 16,
            2.0 / 16, 4.0 / 16, 2.0 / 16,
            1.0 / 16, 2.0 / 16, 1.0 / 16
        );
        color = apply_kernel(kernel);
    } else if (effect == 5) {
        // edge detection
        float kernel[9] = float[](
            1,  1, 1,
            1, -8, 1,
            1,  1, 1
        );
        color = apply_kernel(kernel);
    }


    FragColor = vec4(color, 1.0);
}
