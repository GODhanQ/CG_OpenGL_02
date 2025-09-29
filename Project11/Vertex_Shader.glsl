#version 330 core

layout (location = 0) in vec3 initial_center;
layout (location = 1) in vec3 aColor;
layout (location = 2) in float state;
layout (location = 3) in float expand_angle;
layout (location = 4) in float expand_radius;
layout (location = 5) in vec3 stop_position;
layout (location = 6) in vec3 shrink_center;
layout (location = 7) in float shrink_progress;
layout (location = 8) in float shrink_initial_radius;

out vec3 ourColor; // 프래그먼트 셰이더로 전달될 색상

const float PI = 3.14159265359;

void main()
{
    vec3 final_position;

    if (state < 0.5) { // 확장 상태
        float final_radius = expand_radius;
        float current_angle_rad = radians(expand_angle);
        vec3 offset = vec3(cos(current_angle_rad) * final_radius, sin(current_angle_rad) * final_radius, 0.0);
        final_position = initial_center + offset;
    } else { // 수축 상태
        // 수축 나선 계산
        // 180도에서 시작하여 시계 방향(-)으로 회전
        float shrink_angle = 180.0 - (shrink_progress * 360.0 * 2.0); 
        float shrink_radius = (1.0 - shrink_progress) * shrink_initial_radius;
        
        vec3 shrink_offset = vec3(cos(radians(shrink_angle)) * shrink_radius, sin(radians(shrink_angle)) * shrink_radius, 0.0);

        // 새로 계산된 수축 중심(shrink_center)을 기준으로 수축
        final_position = shrink_center + shrink_offset;
    }

    gl_Position = vec4(final_position, 1.0);
    ourColor = aColor; // 이 라인을 추가해야 합니다.
}