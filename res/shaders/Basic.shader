#shader vertex
#version 330 core

layout(location = 0) in vec4 position;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * position;
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

void main()
{
    color = vec4(0.5, 0.1, 0.3, 1.0);
};