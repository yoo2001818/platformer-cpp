const float PI = 3.1415926535897932384626433832795;

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

float radicalInverse_VdC(uint bits) {
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

float vanDerCorput(uint n, uint base) {
    float invBase = 1.0 / float(base);
    float denom = 1.0;
    float result = 0.0;

    for(uint i = 0u; i < 32u; ++i) {
        if(n > 0u) {
            denom = mod(float(n), 2.0);
            result += denom * invBase;
            invBase = invBase / 2.0;
            n = uint(float(n) / 2.0);
        }
    }

    return result;
}

vec2 hammersley(uint i, uint N) {
    return vec2(float(i) / float(N), radicalInverse_VdC(i));
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
