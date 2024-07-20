#version 330 core
in vec3 vPosition;
out vec4 FragColor;
uniform sampler2D uTexture;

const vec2 invAtan = vec2(0.1591, 0.3183);

void main()
{
  vec3 dir = normalize(vPosition);

  // Run equirectangular mapping
  vec2 uv = vec2(atan(dir.z, dir.x), asin(dir.y));
  uv *= invAtan;
  uv += 0.5;

  FragColor = texture(uTexture, uv);
}
