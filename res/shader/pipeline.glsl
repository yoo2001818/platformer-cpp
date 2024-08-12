struct MaterialInfo {
  vec3 albedo;
  vec3 normal;
  vec3 position;
  float roughness;
  float metalic;
};

/*
vec3 calcBRDF(vec3 L, vec3 V, vec3 N, MaterialInfo mInfo) {
    float roughness = mInfo.roughness * mInfo.roughness;
    vec3 albedo = mix(mInfo.albedo, vec3(0.0), mInfo.metalic);
    vec3 fresnel = mix(vec3(0.04), mInfo.albedo, mInfo.metalic);

    return brdfCookTorr(L, V, N, max(roughness, 0.000001), albedo, fresnel);
}
*/
