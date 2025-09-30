#include "VarANDFunc_02_Shader04.h"

GLint Window_width{ 1000 }, Window_height{ 1000 };

int Rotation_Num{ 3 }, Spin_Speed{ 5 };
float Radius_change_Speed{ 0.001f };
float Max_Angle{ Rotation_Num * 360.0f };
float time_by_sec{ 0.0f };
float rotate_speed{ 200.0f };

GLfloat bgColorR{ 0.3f }, bgColorG{ 0.3f }, bgColorB{ 0.3f };
GLenum drawing_type{ GL_POINTS };