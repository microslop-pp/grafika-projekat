//#shader vertex
#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normal_matrix;

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = normal_matrix * aNormal;
    TexCoords = aTexCoords;
    gl_Position = projection * view * vec4(FragPos, 1.0);
}

//#shader fragment
#version 330 core

out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D texture_diffuse1;
uniform vec3 light_pos;
uniform vec3 light_direction;
uniform vec3 light_color;
uniform vec3 light_color_dir;
uniform vec3 view_pos;
uniform samplerCube shadow_map;
uniform float far_plane;

float point_shadow_calculation(vec3 frag_pos) {
    vec3 frag_to_light = frag_pos - light_pos;
    float closest_depth = texture(shadow_map, frag_to_light).r;
    closest_depth *= far_plane;
    float current_depth = length(frag_to_light);
    float bias = 0.05;
    float shadow = current_depth - bias > closest_depth ? 1.0 : 0.0;
    return shadow;
}

void main() {
    vec3 object_color = texture(texture_diffuse1, TexCoords).rgb;


    //point light:
    //ambient
    float ambient_strength = 0.02;
    vec3 ambient = ambient_strength * light_color;

    //diff
    float diff_strenth = 0.5f;
    vec3 norm = normalize(Normal);
    vec3 light_dir = normalize(light_pos - FragPos);
    float diff = max(dot(norm, light_dir), 0.0);
    vec3 diffuse = diff * light_color * diff_strenth;

    //spec
    float specular_strength = 0.1;
    vec3 view_dir = normalize(view_pos - FragPos);
    vec3 reflect_dir = reflect(-light_dir, norm);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 32);
    vec3 specular = specular_strength * spec * light_color;

    //final
    float shadow = point_shadow_calculation(FragPos);
    // FragColor = vec4(vec3(shadow), 1.0);
    // return;
    vec3 result = (ambient + (1.0 - shadow) * (diffuse + specular)) * object_color;
    //FragColor = vec4(result, 1.0);


    //direkciono:
    //ambient
    float ambient_strength_dir = 0.3;
    vec3 ambient_dir = ambient_strength_dir * light_color_dir;

    //diffuse
    float diff_strenth_dir = 0.5;
    vec3 norm_dir = normalize(Normal);
    vec3 light_dir_dir = normalize(-light_direction);
    float diff_dir = max(dot(norm_dir, light_dir_dir), 0.0);
    vec3 diffuse_dir = diff_dir * light_color_dir * diff_strenth_dir;

    //spec
    float specular_strength_dir = 0.1;
    vec3 view_dir_dir = normalize(view_pos - FragPos);
    vec3 reflect_dir_dir = reflect(-light_dir, norm);
    float spec_dir = pow(max(dot(view_dir_dir, reflect_dir_dir), 0.0), 32);
    vec3 specular_dir = specular_strength_dir * spec_dir * light_color_dir;

    //final
    result += (ambient_dir + diffuse_dir + specular_dir) * object_color;
    FragColor = vec4(result, 1.0);
}