//Name: Defaultlit
//Type: Vertex

#version 450

layout(location = 0) in vec3 inPosition;

layout(set = 0,binding = 0) uniform UniformBufferObject {
    mat4 u_Model;
    mat4 u_View;
    mat4 u_Proj;
} ubo;

layout(location = 0) out vec3 fragColor;

void main() {
    gl_Position = ubo.u_Proj * ubo.u_View * ubo.u_Model * vec4(inPosition, 1.0);
    fragColor = vec3(0.0f,0.0f,1.0f);
}