#version 460 core

#define MAX_WEIGHT 200

out vec4 FragColor;
in vec2 uv;

uniform sampler2D previousScene;
uniform sampler2D rayTracedScene;

uniform int frameIndex;
uniform float iTime;

void main() {
    vec4 prevScene = texture(previousScene, uv);
    vec4 newScene = texture(rayTracedScene, uv);

    if(frameIndex == 1) {
        FragColor = newScene;
    } else {
        vec4 oldScene = prevScene;
        oldScene += newScene;
        newScene = clamp(newScene, 0.0, 1.0);
        FragColor = oldScene;
    }

    // FragColor = newScene;

    // if(frameIndex <= 1) {
    //     FragColor = newScene;
    // } else {
        // prevScene += newScene;
        // prevScene /= 1;
        // FragColor = prevScene;
        // float weight = 1.0 / (frameIndex + 1);
        // // vec4 average = prevScene * (1 - weight) + newScene * weight;
        // vec4 average = vec4(0.3) + prevScene;;
        // FragColor = average;
    // }
}