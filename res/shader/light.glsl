struct PointLight {
  vec3 position;
  vec3 color;
  vec3 intensity;
};

vec3 calcPointLight(out vec3 L, vec3 V, vec3 N, vec3 hitPos, PointLight light) {
  float radius = light.intensity.y;
  L = light.position - hitPos;

  vec3 R = reflect(V, N);
  vec3 centerToRay = dot(L, R) * R - L;
  vec3 closestPos = L +
    centerToRay * clamp(radius / length(centerToRay), 0.0, 1.0);
  L = closestPos;

  float lightDist = length(L);
  L = L / lightDist;

  float power = light.intensity.x;
  float range = light.intensity.z;
  float attenuation = power / (0.0001 + (lightDist * lightDist));
  float window = 1.0;
  if(range > 0.0) {
    window = pow(max(1.0 - pow(lightDist / range, 4.0), 0.0), 2.0);
  }

  float dotNL = max(dot(N, L), 0.0);

  return window * attenuation * dotNL * light.color;
}

vec3 calcEnvironmentMap(vec3 viewPos, MaterialInfo mInfo, sampler2D brdfMap, samplerCube envMap, float lodMax, float power) {
  vec3 albedo = mix(mInfo.albedo, vec3(0.0), mInfo.metalic);
  vec3 fresnel = mix(vec3(0.04), mInfo.albedo, mInfo.metalic);
  float roughness = mInfo.roughness;

  vec3 V = normalize(viewPos - mInfo.position);
  vec3 N = mInfo.normal;
  vec3 R = reflect(-V, N);
  float dotNV = max(dot(N, V), 0.0);
  float lod = roughness * (lodMax - 1.0);
  vec3 envColor = textureLod(envMap, R, lod).rgb * power;
  vec3 F = fresnelSchlickRoughness(dotNV, fresnel, roughness * roughness);
  vec2 envBRDF = texture(brdfMap, vec2(dotNV, roughness)).rg;

  vec3 spec = envColor * (F * envBRDF.x + envBRDF.y);

  vec3 kS = F;
  vec3 kD = vec3(1.0) - kS;

  vec3 irradiance = textureLod(envMap, N, lodMax).rgb * power;

  return kD * albedo * irradiance + spec;
}
