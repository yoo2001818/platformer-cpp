#version 330 core
in vec3 vPosition;
out vec4 FragColor;
uniform float uResolution;
uniform int uMipLevel;
uniform int uMaxMipLevel;
uniform samplerCube uSource;

#include "res/shader/pbr.glsl"

vec3 runSample(vec3 direction, float roughness, float resolution) {
  vec3 N = normalize(direction);
  vec3 R = N;
  vec3 V = R;

  const uint SAMPLE_COUNT = uint(256);
  float totalWeight = 0.0;
  vec3 prefilteredColor = vec3(0.0);
  for(uint i = uint(0); i < SAMPLE_COUNT; ++i) {
    vec2 Xi = hammersley(i, SAMPLE_COUNT);
    vec3 H = importanceSampleGGX(Xi, N, roughness);

    float dotHV = max(dot(H, V), 0.0);

    vec3 L = normalize(2.0 * dot(H, V) * H - V);

    float dotNH = max(dot(N, H), 0.0);

    float D = distributionGGX(dotNH, roughness);
    float pdf = (D * dotNH / (4.0 * dotHV)) + 0.0001;

    float saTexel = 4.0 * PI / (6.0 * resolution * resolution);
    float saSample = 1.0 / (float(SAMPLE_COUNT) * pdf + 0.0001);

    float mipLevel = 0.0;
    if(roughness > 0.0) {
      mipLevel = 0.5 * log2(saSample / saTexel);
    }

    float dotNL = max(dot(N, L), 0.0);
    if(dotNL > 0.0) {
      prefilteredColor += textureLod(uSource, L, min(mipLevel, uMaxMipLevel - 1.0)).rgb * dotNL;
      totalWeight += dotNL;
    }
  }
  prefilteredColor = prefilteredColor / totalWeight;

  return prefilteredColor;
}

vec3 runIrradianceSample(vec3 normal) {
  vec3 irradiance = vec3(0.0);

  vec3 up = vec3(0.0, 1.0, 0.0);
  vec3 right = normalize(cross(up, normal));
  up = normalize(cross(normal, right));

  float sampleDelta = 0.025;
  float nrSamples = 0.0;
  for(int phiI = 0; phiI < 100; ++phiI) {
    float phi = float(phiI) / 100.0 * 2.0 * PI;
    for(int thetaI = 0; thetaI < 25; ++thetaI) {
      float theta = float(thetaI) / 25.0 * 0.5 * PI;
            // spherical to cartesian (in tangent space)
      vec3 tangentSample = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
            // tangent space to world
      vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal;

      irradiance += textureLod(uSource, sampleVec, uMaxMipLevel).rgb * cos(theta) * sin(theta);
      nrSamples += 1.0;
    }
  }
  irradiance = PI * irradiance * (1.0 / nrSamples);
  return irradiance;
}

void main() {
  vec3 dir = normalize(vPosition);

  vec3 result;
  if(uMipLevel < uMaxMipLevel) {
    // radiance calculation
    result = runSample(dir, pow(min(float(uMipLevel) / (float(uMaxMipLevel) - 1.0), 1.0), 2.0), uResolution);
  } else if(uMipLevel == uMaxMipLevel) {
    // irradiance calculation
    result = runIrradianceSample(dir);
  } else {
    result = vec3(0.0);
  }
  FragColor = vec4(result, 1.0);
}
