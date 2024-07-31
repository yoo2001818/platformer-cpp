#version 330 core
in vec3 vPosition;
out vec4 FragColor;
uniform samplerCube uTexture;
uniform float uLod;

void main() {
  vec3 color = textureLod(uTexture, vPosition, uLod).rgb;
  FragColor = vec4(color, 1.0);
}
