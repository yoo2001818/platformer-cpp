#version 330 core
in vec3 vPosition;
out vec4 FragColor;

void main()
{
  FragColor = vec4(vPosition * 0.5f + 0.5f, 1.0f);
}


