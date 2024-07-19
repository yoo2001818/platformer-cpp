#version 330 core
layout (location = 0) in vec3 aPosition;
out vec3 vPosition;
uniform mat4 uModel;

void main()
{
   gl_Position = vec4(aPosition.xy, 1.0, 1.0);
   vPosition = (uModel * vec4(aPosition.xy, 1.0, 1.0)).xyz;
}

