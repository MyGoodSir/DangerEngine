#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 FragPos;

uniform sampler2D t_diffuse1;
uniform sampler2D t_specular1;
uniform sampler2D t_normal1;
uniform vec3 light_pos;
uniform vec3 light_col;
uniform vec3 view_pos;


void main()
{
    vec3 norm = normalize(vec3(texture(t_normal1, TexCoords)));
    vec3 l_dir = normalize(light_pos - FragPos);
    float diffuse_strength = max(dot(norm, l_dir), 0.);
    float spec_strength = 0.5;
    vec3 v_dir = normalize(view_pos - FragPos);
    vec3 r_dir = reflect(-l_dir, norm);

    float spec = pow(max(dot(v_dir, r_dir), 0.), 32);

    vec3 diffuse  = diffuse_strength * light_col * vec3(texture(t_diffuse1, TexCoords));  
    vec3 specular = spec_strength * spec * light_col * vec3(texture(t_specular1, TexCoords));
    FragColor = vec4(diffuse + specular, 1.0);   
}