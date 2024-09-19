layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in vec4 aTangent;
#ifdef USE_VERTEX_COLOR
in vec4 aColor;
#endif
in mat4 aModel;
out vec3 vPosition;
out vec3 vNormal;
out vec2 vTexCoord;
#ifdef USE_VERTEX_COLOR
out vec4 vColor;
#endif
uniform mat4 uView;
uniform mat4 uProjection;

void main() {
  gl_Position = uProjection * uView * aModel * vec4(aPosition, 1.0);
  vNormal = (aModel * vec4(aNormal, 0.0)).xyz;
  vPosition = (aModel * vec4(aPosition, 1.0)).xyz;
  vTexCoord = aTexCoord;
  #ifdef USE_VERTEX_COLOR
  vColor = aColor;
  #endif
}
