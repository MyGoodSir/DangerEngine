#version 330 core
layout (location = 0) in vec2 pos;
layout (location = 1) in vec3 col;
layout (location = 2) in vec2 texCoord;

out vec3 fcol;
out vec2 vTexCoord;

void main()
{
    gl_Position = vec4(pos, 1.0, 1.0);
    fcol = col;
    vTexCoord = texCoord;
}