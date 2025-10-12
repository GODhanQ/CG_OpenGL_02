#version 330 core
in vec3 out_Color;
out vec4 FragColor;

// 모드 제어를 위한 uniform 변수들
uniform bool u_IsPicking = false;       // Picking 모드 활성화 여부
uniform vec3 u_PickingColor;            // Picking 시 사용할 ID 색상

uniform bool u_UseOverrideColor = false; // 색상 덮어쓰기 활성화 여부
uniform vec3 u_OverrideColor;           // 덮어쓸 색상 (예: 선택 하이라이트)

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