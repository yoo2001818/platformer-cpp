#version 330 core
layout (location = 0) in vec3 aPosition;
out vec3 vPosition;
uniform mat4 uTransform;

void main()
{
   gl_Position = vec4(aPosition.xy, 1.0, 1.0);
   vPosition = (uTransform * vec4(aPosition.xy, 1.0, 1.0)).xyz;
}

