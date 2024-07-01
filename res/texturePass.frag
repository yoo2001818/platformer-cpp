#version 330 core
in vec2 vTexCoord;
out vec4 FragColor;
uniform sampler2D uDiffuse;

void main()
{
    vec3 color = texture2D(uDiffuse, vTexCoord).rgb;
    FragColor = vec4(color, 1.0f);
}
