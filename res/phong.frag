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
uniform sampler2D uDiffuse;
void main()
{
    vec3 color = texture2D(uDiffuse, vTexCoord).rgb;
    vec3 result = vec3(0.0);
    vec3 ambient = vec3(0.1);
    result += ambient * color;
    vec3 normal = normalize(vNormal);
    for (int i = 0; i < 8; i += 1) {
        if (i > uLightCount) break;
        vec3 lightPos = (uView * vec4(uLightPositions[i].xyz, 1.0)).xyz;
        vec3 lightDir = normalize(lightPos - vPosition);
        result += max(dot(vNormal, lightDir), 0.0)* uLightColors[i] * color;
        vec3 viewDir = normalize(-vPosition);
        vec3 reflectDir = reflect(-lightDir, normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
        result += 0.5 * spec * uLightColors[i];
    }
    FragColor = vec4(result, 1.0f);
}
