#version 330 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec2 aCenter;

out vec2 vTexCoord;

uniform vec2 uScale;

void main() {
  vTexCoord = aTexCoord;
  /*
  mat4 mvp = uProjection * uView * uModel;
  // Determine the NDC of the center position
  vec4 centerPos = mvp * vec4(0.0, 0.0, 0.0, 1.0);
  centerPos /= centerPos.w;
  */
  vec2 centerPos = aCenter;
  // Calculate in screen space...
  gl_Position = vec4(centerPos.xy + aPosition.xy * uScale, 0.0, 1.0);
}
