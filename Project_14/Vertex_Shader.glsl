#version 330 core

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_Color;

uniform vec3 rotation_axis;
uniform float rotation_angle;
uniform float scale_factor;
uniform vec3 triangle_center; // 삼각형의 중심점

out vec3 out_Color;

void main()
{
    vec3 pos = in_position - triangle_center;
    pos = pos * scale_factor;
    pos = pos + triangle_center;

    vec2 pivot = rotation_axis.xy;
    pos = pos - vec3(pivot, 0.0);

    float angle_rad = radians(rotation_angle);
    float cos_angle = cos(angle_rad);
    float sin_angle = sin(angle_rad);

    vec3 rotated_pos;
    rotated_pos.x = pos.x * cos_angle - pos.y * sin_angle;
    rotated_pos.y = pos.x * sin_angle + pos.y * cos_angle;
    rotated_pos.z = pos.z;

    pos = rotated_pos + vec3(pivot, 0.0);

    gl_Position = vec4(pos, 1.0);
    out_Color = in_Color;
}