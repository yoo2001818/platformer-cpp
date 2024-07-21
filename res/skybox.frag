#version 330 core
in vec2 vPosition;
out vec4 FragColor;
uniform mat4 uInverseView;
uniform mat4 uInverseProjection;
uniform samplerCube uTexture;
void main()
{
    vec4 viewPos = uInverseProjection * vec4(vPosition.xy, 1.0, 1.0);
    viewPos /= viewPos.w;
    vec3 dir = (uInverseView * vec4(normalize(viewPos.xyz), 0.0)).xyz;
    vec3 color = texture(uTexture, dir).rgb;
    // TODO: Tonemapping functions
    vec3 tonemappedColor = pow(color, vec3(1.0 / 2.2));
    FragColor = vec4(tonemappedColor, 1.0f);
}

