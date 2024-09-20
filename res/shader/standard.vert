layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec3 aNormal;
#ifdef USE_NORMAL_TEXTURE
layout(location = 3) in vec4 aTangent;
#endif
#ifdef USE_VERTEX_COLOR
in vec4 aColor;
#endif
#ifdef USE_INSTANCING
in mat4 aModel;
#endif
#ifdef USE_ARMATURE
in ivec4 aBoneIds;
in vec4 aBoneWeights;
#endif
out vec3 vPosition;
out vec3 vNormal;
out vec2 vTexCoord;
#ifdef USE_NORMAL_TEXTURE
out vec4 vTangent;
#endif
#ifdef USE_VERTEX_COLOR
out vec4 vColor;
#endif
#ifndef USE_INSTANCING
uniform mat4 uModel;
#endif
uniform mat4 uView;
uniform mat4 uProjection;
#ifdef USE_ARMATURE
uniform samplerBuffer uBoneMatrices;
#endif

void main() {
  #ifdef USE_INSTANCING
  mat4 model = aModel;
  #else
  mat4 model = uModel;
  #endif
  #ifdef USE_ARMATURE
  mat4 armatureMat = mat4(0.0);
  for(int i = 0; i < 4; ++i) {
    float weight = aBoneWeights[i];
    if(weight > 0.0) {
      mat4 target = mat4(texelFetch(uBoneMatrices, aBoneIds[i] * 4), texelFetch(uBoneMatrices, aBoneIds[i] * 4 + 1), texelFetch(uBoneMatrices, aBoneIds[i] * 4 + 2), texelFetch(uBoneMatrices, aBoneIds[i] * 4 + 3));
      armatureMat += target * weight;
    }
  }
  model = model * armatureMat;
  #endif
  gl_Position = uProjection * uView * model * vec4(aPosition, 1.0);
  vNormal = (model * vec4(aNormal, 0.0)).xyz;
  vPosition = (model * vec4(aPosition, 1.0)).xyz;
  vTexCoord = aTexCoord;
  #ifdef USE_NORMAL_TEXTURE
  vTangent = vec4((model * vec4(aTangent.xyz, 0.0)).xyz, aTangent.w);
  #endif
  #ifdef USE_VERTEX_COLOR
  vColor = aColor;
  #endif
}
