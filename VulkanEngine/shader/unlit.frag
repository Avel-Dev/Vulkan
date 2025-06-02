#version 450

layout(location = 0) in vec3 fragColor;  // Input from vertex shader
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;  // Output color

void main() {
	outColor = vec4(fragColor, 1.0);  // Set output color to the input color with full opacity)
}