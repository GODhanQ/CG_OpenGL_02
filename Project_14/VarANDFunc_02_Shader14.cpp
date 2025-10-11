#include "VarANDFunc_02_Shader14.h"

GLint Window_width{ 700 }, Window_height{ 700 };

bool transforming{ false }, is_scaling_animation_active{ false }, is_shrinking{ false };
int current_animating_triangle{ 0 };		// 0 ~ 3
float delta_angle{ 0.005f }, sum_angle{ 0.0f }, animation_progress{ 0.0f };
glm::vec3 rotation_axis_value{ 0.0f, 0.0f, 1.0f };

float first_range{ 0.4f }, second_range{ 0.8f };
float third_range{ 0.4f }, fourth_range{ 0.1f };

glm::vec3 vertices_sq[4] = {
		{first_range, first_range, 0.0f},	// right top		index : 0
		{-first_range, first_range, 0.0f},	// left top			index : 1
		{-first_range, -first_range, 0.0f},	// left bottom		index : 2
		{first_range, -first_range, 0.0f}	// right bottom		index : 3
};
glm::vec3 vertices_dia[4] = {
	{second_range, 0.0f, 0.0f},			// right			index : 4
	{0.0f, second_range, 0.0f},			// top				index : 5
	{-second_range, 0.0f, 0.0f},		// left				index : 6
	{0.0f, -second_range, 0.0f}			// bottom			index : 7
};

unsigned int indices_tri[12] = {
	0, 5, 1,		// top tirangle
	1, 6, 2,		// left triangle
	2, 7, 3,		// bottom triangle
	3, 4, 0			// right triangle
};