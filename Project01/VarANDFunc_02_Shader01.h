#pragma once
#include <GL/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <tuple>
#include <random>
#include <chrono>

struct Vertex_glm {
	glm::vec3 position;
	glm::vec3 color;
};

extern GLint Window_width, Window_height;

extern float vPositionList[];
extern float vColorList[];
extern unsigned int index[];
extern float Triangle_range;

extern bool DrawPoint_mode, DrawLine_mode, DrawTriangle_mode, DrawSquare_mode;
extern int Current_Diagram_Count, Selected_Diagram;

char* filetobuf(const char* file);
void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();
GLvoid drawScene();
GLvoid Reshape(int w, int h);
void Keyboard(unsigned char key, int x, int y);
void MouseClick(int button, int state, int x, int y);
void INIT_BUFFER();
void UPDATE_BUFFER();
std::pair<float, float> ConvertMouseWxy2GLxy(int x, int y);
void CreatePointAtOrigin(float ogl_x, float ogl_y);
void CreateLineAtOrigin(float ogl_x, float ogl_y);
void CreateTriangleAtOrigin(float ogl_x, float ogl_y);
void CreateSquareAtOrigin(float ogl_x, float ogl_y);