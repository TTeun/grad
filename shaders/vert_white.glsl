#version 330
layout(location = 0) in vec2 position;
layout(location = 1) in vec3 color;
out vec4 vColor;

void main()

{
  gl_Position = vec4(position, 1.0, 1.0);
  vColor = vec4(1.f, 1.f, 1.f, 1.0f);
}
