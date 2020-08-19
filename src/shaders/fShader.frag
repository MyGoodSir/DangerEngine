#version 410 core
in vec3 fragCol;
out vec4 color;

void main()
{
    color = vec4(fragCol.x,fragCol.y,fragCol.z, 1.0f);
}