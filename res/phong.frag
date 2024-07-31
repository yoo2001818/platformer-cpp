#version 330 core
in vec3 vPosition;
in vec3 vNormal;
in vec2 vTexCoord;
out vec4 FragColor;
uniform vec4 uLightPositions[8];
uniform vec3 uLightColors[8];
uniform vec3 uLightRanges[8];
uniform vec3 uColor;
uniform int uLightCount;
uniform float uRoughness;
uniform float uMetalic;
uniform mat4 uView;
uniform vec3 uViewPos;

#ifdef USE_DIFFUSE_TEXTURE
uniform sampler2D uDiffuse;
#endif
#ifdef USE_ENVIRONMENT_MAP
uniform sampler2D uBRDFMap;
uniform samplerCube uEnvironmentMap;
uniform vec2 uEnvironmentMapSize;
#endif

const float PI = 3.1415926535897932384626433832795;

struct MaterialInfo {
    vec3 albedo;
    vec3 normal;
    vec3 position;
    vec3 hardNormal;
    float depth;
    float roughness;
    float metalic;
};

float distributionGGX(float dotNM, float a) {
    float a2 = a * a;
    float dotNM2 = dotNM * dotNM;
    float nom = dotNM * a2;
    float denom = (dotNM2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    return nom / max(denom, 0.0000001);
}

float pdfDistributionGGX(float dotNM, float dotOM, float a) {
    float nom = distributionGGX(dotNM, a) * dotNM;
    float denom = 4.0 * dotOM;
    return nom / denom;
}

float lambdaGGX(float a) {
    float a2 = a * a;
    float nom = sqrt(1.0 + 1.0 / a2) - 1.0;
    return nom / 2.0;
}

float geometrySchlickGGX(float dotNV, float roughness) {
    float a = roughness;
    float k = (a * a) / 2.0;
    float nom = dotNV;
    float denom = dotNV * (1.0 - k) + k;

    return nom / denom;
}

float geometrySmith(float roughness, float dotNV, float dotNL) {
    float ggx1 = geometrySchlickGGX(dotNL, roughness);
    float ggx2 = geometrySchlickGGX(dotNV, roughness);
    return ggx1 * ggx2;
}

vec3 fresnelSchlickRoughness(float dotNL, vec3 F0, float roughness) {
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - dotNL, 5.0);
}

vec3 fresnelSchlick(float dotHV, vec3 F0) {
    return F0 + (1.0 - F0) * pow(1.0 - dotHV, 5.0);
}

vec3 specCookTorr(float D, vec3 F, float G, float dotNL, float dotNV) {
    vec3 nom = D * G * F;
    float denom = 4.0 * max(dotNL, 0.0) * max(dotNV, 0.0);
    return nom / max(denom, 0.001);
}

vec3 specCookTorrGGX(
    vec3 L,
    vec3 V,
    vec3 N,
    float roughness,
    vec3 albedo,
    vec3 reflection,
    out vec3 F
) {
    float dotNL = max(dot(N, L), 0.0);
    float dotNV = max(dot(N, V), 0.0);

    vec3 H = normalize(V + L);

    float dotNH = max(dot(N, H), 0.0);
    float dotHV = max(dot(H, V), 0.0);

    float D = distributionGGX(dotNH, roughness);
    F = fresnelSchlick(dotHV, reflection);
    float G = geometrySmith(roughness, dotNV, dotNL);

    return specCookTorr(D, F, G, dotNL, dotNV);
}

vec3 brdfCookTorr(
    vec3 L,
    vec3 V,
    vec3 N,
    float roughness,
    vec3 albedo,
    vec3 reflection
) {
    float dotNL = max(dot(N, L), 0.0);
    vec3 F;
    vec3 spec = specCookTorrGGX(L, V, N, roughness, albedo, reflection, F);

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;

    return dotNL * albedo / PI + spec;
}

float vanDerCorput(int n, int base) {
    float invBase = 1.0 / float(base);
    float denom = 1.0;
    float result = 0.0;

    for(int i = 0; i < 32; ++i) {
        if(n > 0) {
            denom = mod(float(n), 2.0);
            result += denom * invBase;
            invBase = invBase / 2.0;
            n = int(float(n) / 2.0);
        }
    }

    return result;
}

vec2 hammersley(int i, int N) {
    return vec2(float(i) / float(N), vanDerCorput(i, 2));
}

vec2 hammersleyFromMap(sampler2D map, int i, int N) {
    float pos = float(i) / float(N);
    vec4 tex = texture2D(map, vec2(pos, 0.5));
    float value = dot(tex.rgb, 1.0 / vec3(1.0, 255.0, 65025.0));
    return vec2(pos, value);
}

vec3 importanceSampleGGX(vec2 Xi, vec3 N, float roughness) {
    float a = roughness * roughness;

    float phi = 2.0 * PI * Xi.x;
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a * a - 1.0) * Xi.y));
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

    // from spherical coordinates to cartesian coordinates
    vec3 H;
    H.x = cos(phi) * sinTheta;
    H.y = sin(phi) * sinTheta;
    H.z = cosTheta;

    // from tangent-space vector to world-space sample vector
    vec3 up = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent = normalize(cross(up, N));
    vec3 bitangent = cross(N, tangent);

    vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
    return normalize(sampleVec);
}

vec3 calcBRDF(vec3 L, vec3 V, vec3 N, MaterialInfo mInfo) {
    float roughness = mInfo.roughness * mInfo.roughness;
    vec3 albedo = mix(mInfo.albedo, vec3(0.0), mInfo.metalic);
    vec3 fresnel = mix(vec3(0.04), mInfo.albedo, mInfo.metalic);

    return brdfCookTorr(L, V, N, max(roughness, 0.000001), albedo, fresnel);
}

struct PointLight {
    vec3 position;
    vec3 color;
    vec3 intensity;
};

vec3 calcPointLight(
    out vec3 L,
    vec3 V,
    vec3 N,
    vec3 hitPos,
    PointLight light
) {
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

void main() {
    MaterialInfo mInfo;
    #ifdef USE_DIFFUSE_TEXTURE
    mInfo.albedo = pow(texture(uDiffuse, vTexCoord).rgb, vec3(2.2));
    #else
    mInfo.albedo = uColor;
    #endif
    mInfo.normal = normalize(vNormal);
    mInfo.position = vPosition;
    mInfo.roughness = uRoughness;
    mInfo.metalic = uMetalic;

    vec3 result = vec3(0.0);
    vec3 ambient = vec3(0.0);
    result += ambient * mInfo.albedo;
    // point lights
    for(int i = 0; i < 8; i += 1) {
        if(i > uLightCount)
            break;
        PointLight light;
        light.position = uLightPositions[i].xyz;
        light.color = uLightColors[i];
        light.intensity = uLightRanges[i];

        vec3 L;
        vec3 V = normalize(uViewPos - mInfo.position);
        vec3 N = mInfo.normal;
        result += calcPointLight(L, V, N, mInfo.position, light) *
            calcBRDF(L, V, N, mInfo);
    }
    #ifdef USE_ENVIRONMENT_MAP
    result += calcEnvironmentMap(uViewPos, mInfo, uBRDFMap, uEnvironmentMap, uEnvironmentMapSize.x, uEnvironmentMapSize.y);
    #endif
    // TODO: Tonemapping functions
    vec3 tonemappedColor = pow(result, vec3(1.0 / 2.2));
    FragColor = vec4(tonemappedColor, 1.0);
}
