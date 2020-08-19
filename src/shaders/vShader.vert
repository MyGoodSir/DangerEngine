#version 410 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 vertCol;
uniform mat4 MVP;

out vec3 fragCol;

void main()
{
    gl_Position = MVP * vec4(pos.x, pos.y, pos.z, 1.0);
    fragCol = vertCol;
}