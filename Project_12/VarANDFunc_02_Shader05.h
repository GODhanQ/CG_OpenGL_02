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

enum ShapeType {
	LINE = 0,
	TRIANGLE = 1,
	QUAD = 2,
	PENTAGON = 3
};

namespace ShapeAngles {
	const float LINE_ANGLES[6] = { 45.0f, 225.0f, 225.0f, 225.0f, 225.0f, 255.0f };
	const float TRIANGLE_ANGLES[6] = { 90.0f, 210.0f, 330.0f, 330.0f, 330.0f, 90.0f };
	const float QUAD_ANGLES[6] = { 45.0f, 135.0f, 225.0f, 315.0f, 315.0f, 45.0f };
	const float PENTAGON_ANGLES[6] = { 18.0f, 90.0f, 162.0f, 234.0f, 306.0f, 18.0f };
}

struct Vertex_glm {
	glm::vec3 position;
	glm::vec3 color;
};

struct Shape {
	Vertex_glm vertices[6];
	glm::vec3 center;
	glm::vec2 ver_mov_dir[6];
	bool is_in_progress;
	// 0 : line, 1 : triangle, 2 : rectangle, 3 : heptagon, 
	int current_state;
	GLuint draw_mode;

	Shape();
	Shape(glm::vec3 c);
	Shape(glm::vec3 c, int state);
};
struct ShapeData {
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> colors;
	std::vector<glm::vec3> centers;
	std::vector<glm::vec2> ver_mov_dirs;
	std::vector<GLuint> draw_modes;
	std::vector<int> current_states;
	std::vector<bool> is_in_progress;

	bool empty() const { 
		return positions.empty();
	}
	void clear() {
		positions.clear();
		colors.clear();
		centers.clear();
		ver_mov_dirs.clear();
		draw_modes.clear();
		current_states.clear();
		is_in_progress.clear();
	}
};

class ShapeManager {
public:
	ShapeData shape_data;
	std::vector<Shape> all_shapes;

	void PrepareShapeData();
};

extern GLint Window_width, Window_height;

extern GLuint shared_ibo_line_id[2], shared_ibo_fan_id[7];
extern float radius_for_all, radius_for_one;
extern float moving_speed_for_all, moving_speed_for_one;

// this flag is for draw all mode shapes ( quadrant shapes )
// if all_drawing mode is on, is_line2triangle, is_triangle2rectangle, is_rectangle2heptagon, is_heptagon2line are ignored
extern bool is_all_drawing;

// this flags are for draw one mode shapes ( transforming shapes )
// this flags can be known ShapeData::current_state and ShapeData::is_in_progress
//extern bool is_line2triangle, is_triangle2rectangle, is_rectangle2heptagon, is_heptagon2line;
//extern bool is_transforming;

// 0: line, 1: triangle, 2: rectangle, 3: heptagon
// during all_drawing mode, this variable is ignored { -1 }
extern int current_shape_state;

extern bool do_second_process;

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