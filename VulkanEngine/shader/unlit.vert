#version 450


layout(binding = 0) uniform UniformBufferObject {
    mat4 u_Model;
    mat4 u_View;
    mat4 u_Proj;
} ubo;


layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

void main() {
    gl_Position = ubo.u_Proj * ubo.u_View * ubo.u_Model * vec4(inPosition, 1.0);
    fragColor = inColor;
    fragTexCoord = vec2(inTexCoord.x,inTexCoord.y);
}