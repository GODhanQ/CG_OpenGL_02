#version 330 core
in vec3 out_color;
out vec4 FragColor;

uniform bool u_IsPicking;
uniform vec3 u_PickingColor;

uniform bool u_IsOutline;

void main()
{
    if (u_IsPicking) {
        FragColor = vec4(u_PickingColor, 1.0);
    } else if (u_IsOutline) {
        FragColor = vec4(1.0, 1.0, 0.0, 1.0);
    } else {
        FragColor = vec4(out_color, 1.0);
    }
}