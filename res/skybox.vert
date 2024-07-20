#version 330 core
layout (location = 0) in vec3 aPosition;
out vec2 vPosition;

void main()
{
   gl_Position = vec4(aPosition.xy, 1.0, 1.0);
   vPosition = aPosition.xy;
}

