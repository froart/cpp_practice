#version 450

layout(location = 0) in vec3 fragColor; // location = 0 is an index of the 0-th framebuffer

layout(location = 0) out vec3 outColor;

void main() {
  outColor = vec4(fragColor, 1.0);
}
