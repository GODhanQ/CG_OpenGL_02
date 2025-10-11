#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 ourColor;

void main()
{
    gl_Position = vec4(aPos, 1.0);
    // 점의 크기를 설정합니다.
    gl_PointSize = 10.0; 
    ourColor = aColor;
}