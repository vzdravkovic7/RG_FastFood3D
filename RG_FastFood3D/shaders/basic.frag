#version 330 core

in vec3 vNormal;
in vec3 vFragPos;
in vec2 vTex;

out vec4 FragColor;

uniform vec3 uViewPos;

struct Light {
    vec3 pos;
    vec3 kA;
    vec3 kD;
    vec3 kS;
};

uniform Light uLight;

struct Material {
    vec3 kA;
    vec3 kD;
    vec3 kS;
    float shine;
};

uniform Material uMaterial;

void main() {
    vec3 N = normalize(vNormal);
    vec3 L = normalize(uLight.pos - vFragPos);
    vec3 V = normalize(uViewPos - vFragPos);
    vec3 R = reflect(-L, N);

    float diff = max(dot(N, L), 0.0);
    float spec = pow(max(dot(V, R), 0.0), uMaterial.shine);

    vec3 ambient  = uLight.kA * uMaterial.kA;
    vec3 diffuse  = uLight.kD * (diff * uMaterial.kD);
    vec3 specular = uLight.kS * (spec * uMaterial.kS);

    FragColor = vec4(ambient + diffuse + specular, 1.0);
}
