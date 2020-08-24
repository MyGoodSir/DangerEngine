#version 330 core
in vec3 fcol;
in vec2 vTexCoord;
out vec4 color;

uniform vec4 uCol;
uniform sampler2D uTex;

void main()
{
    color = texture(uTex, vTexCoord) * uCol * vec4(fcol, 1.0);
}