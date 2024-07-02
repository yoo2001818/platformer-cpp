#version 330 core
in vec2 vTexCoord;
out vec4 FragColor;

void main()
{
    FragColor = vec4(vTexCoord, 0.0f, 1.0f);
}

