#version 460 core

in vec2 uv;
out vec4 FragColor;
uniform sampler2D pixels;

void main() {
    FragColor = texture(pixels, uv);
}
