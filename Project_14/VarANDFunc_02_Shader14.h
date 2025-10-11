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

extern GLint Window_width, Window_height;

extern bool transforming, is_scaling_animation_active, is_shrinking;
extern int current_animating_triangle;
extern float delta_angle, sum_angle, animation_progress;
extern glm::vec3 rotation_axis_value;

extern float first_range, second_range;
extern float third_range, fourth_range;

extern glm::vec3 vertices_sq[4];
extern glm::vec3 vertices_dia[4];
extern unsigned int indices_tri[12];

GLvoid drawScene();
GLvoid Reshape(int w, int h);

void KeyBoard(unsigned char key, int x, int y);
void MouseClick(int button, int state, int x, int y);
std::pair<float, float> ConvertScreenToOpenGL(int screen_x, int screen_y);

void INIT_BUFFER();
void UPDATE_BUFFER();

char* filetobuf(const char* file);
GLuint make_shaderProgram(const char* vertPath, const char* fragPath);

void SetupGeometry(float sq_range, float dia_range);
void Animate();