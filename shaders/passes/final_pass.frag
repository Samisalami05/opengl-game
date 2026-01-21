#version 330

in vec2 UV;

out vec4 FragColor;

uniform sampler2D screen;

void main() {
	FragColor = texture(screen, UV);
	//FragColor = vec4(UV, 0.0f, 1.0f);
}
