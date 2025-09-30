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
#include <deque>
#include <tuple>
#include <random>
#include <chrono>
#include <algorithm>
#include <map>
#define NOMINMAX
#include <windows.h>


struct Vertex_glm {
	glm::vec3 position;
	glm::vec3 color;
};

class Shape {
	GLint offset;
	GLint index_count;
	bool is_in_progress;
	// 0 : line, 1 : triangle, 2 : rectangle, 3 : heptagon, 
	int current_state;
	//GLint base_vertex_index;
};

extern GLint Window_width, Window_height;

extern bool is_all_drawing, is_line2triangle, is_triangle2rectangle, is_rectangle2heptagon, is_heptagon2line;
extern bool is_transforming;

GLvoid drawScene();
GLvoid Reshape(int w, int h);

void KeyBoard(unsigned char key, int x, int y);
void MouseClick(int button, int state, int x, int y);
std::pair<float, float> ConvertScreenToOpenGL(int screen_x, int screen_y);

void INIT_BUFFER();
void UPDATE_BUFFER();

char* filetobuf(const char* file);
void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();

void Transform();

void AllDrawing();

void Line2Triangle();
void Triangle2Rectangle();
void Rectangle2Heptagon();
void Heptagon2Line();