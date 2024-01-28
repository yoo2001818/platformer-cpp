#version 330 core
in vec3 vPosition;
in vec3 vNormal;
out vec4 FragColor;
uniform mat4 uView;
void main()
{
    vec3 normal = normalize(vNormal);
    FragColor = vec4(normal * 0.5 + 0.5, 1.0f);
}

