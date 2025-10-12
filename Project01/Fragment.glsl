#version 330 core
in vec3 out_Color;
out vec4 FragColor;

// ��� ��� ���� uniform ������
uniform bool u_IsPicking = false;       // Picking ��� Ȱ��ȭ ����
uniform vec3 u_PickingColor;            // Picking �� ����� ID ����

uniform bool u_UseOverrideColor = false; // ���� ����� Ȱ��ȭ ����
uniform vec3 u_OverrideColor;           // ��� ���� (��: ���� ���̶���Ʈ)

void main()
{
    if (u_IsPicking) {
        FragColor = vec4(u_PickingColor, 1.0);
    } else if (u_UseOverrideColor) {
        FragColor = vec4(u_OverrideColor, 1.0);
    } else {
        FragColor = vec4(out_Color, 1.0);
    }
}