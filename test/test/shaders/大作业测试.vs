#version 330 core
layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec2 TexCoords;
layout(location = 3) in vec3 Position2;
layout(location = 4) in vec3 Normal2;
layout(location = 5) in vec2 TexCoords2;

out vec2 outTexCoord;
out vec3 outNormal;
out vec3 outFragPos;

uniform float factor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float morphFactor;

float smoothstep(float edge0, float edge1, float x) {
    float t = clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0);
    return t * t * (3.0 - 2.0 * t);
}

void main() {
    vec3 pos = mix(Position, Position2, smoothstep(0.0, 1.0, morphFactor));
    vec3 norm = mix(Normal, Normal2, smoothstep(0.0, 1.0, morphFactor));
    vec2 tex = mix(TexCoords, TexCoords2, smoothstep(0.0, 1.0, morphFactor));

    gl_Position = projection * view * model * vec4(pos, 1.0);

    outFragPos = vec3(model * vec4(pos, 1.0));
    outTexCoord = tex;
    outNormal = mat3(transpose(inverse(model))) * norm;
}
