#version 330 core
in vec3 vPosition;
out vec4 FragColor;
uniform samplerCube uTexture;
void main()
{
    vec3 color = texture(uTexture, vPosition).rgb;
    FragColor = vec4(color, 1.0f);
}

