#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTex;

uniform mat4 uM;
uniform mat4 uV;
uniform mat4 uP;

out vec3 vNormal;
out vec3 vFragPos;
out vec2 vTex;

void main() {
    vFragPos = vec3(uM * vec4(aPos, 1.0));
    vNormal = mat3(transpose(inverse(uM))) * aNormal;
    vTex = aTex;

    gl_Position = uP * uV * vec4(vFragPos, 1.0);
}
