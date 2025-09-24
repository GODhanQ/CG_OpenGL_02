#ifndef VARANDFUNC_02_SHADER01_H
#endif VARANDFUNC_02_SHADER01_H

#define _CRT_SECURE_NO_WARNINGS
#include "VarANDFunc_02_Shader01.h"

GLint Window_width{ 1000 }, Window_height{ 1000 };
float Triangle_range{ 0.05f };

bool DrawPoint_mode{ false }, DrawLine_mode{ false }, DrawTriangle_mode{ false }, DrawSquare_mode{ false };
int Current_Diagram_Count{}, Selected_Diagram{ -1 };

struct Vertex {
	std::tuple<float, float, float> position;
	std::tuple<float, float, float> color;
};

float vPositionList_const[] = {
	0.5f, 0.5f, 0.0f, // 우측 상단
	0.5f, -0.5f, 0.0f, // 우측 하단
	-0.5f, -0.5f, 0.0f, // 좌측 하단
	-0.5f, 0.5f, 0.0f // 좌측 상단
};
float vColorList_const[] = {
	1.0f, 0.0f, 0.0f, // 우측 상단	R
	0.0f, 1.0f, 0.0f, // 우측 하단	G
	0.0f, 0.0f, 1.0f, // 좌측 하단	B
	1.0f, 1.0f, 0.0f // 좌측 상단	Y
};

unsigned int index[] = {
	0, 1, 3, // 첫 번째 삼각형
	1, 2, 3 // 두 번째 삼각형
	//0, 1, 2, 3 // 사각형
};

