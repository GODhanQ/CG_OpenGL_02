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

namespace ShapeAngles {
	const float LINE_ANGLES[] = { 45.0f, 225.0f, 225.0f, 225.0f, 225.0f, 255.0f };
	const float TRIANGLE_ANGLES[] = { 90.0f, 210.0f, 330.0f, 330.0f, 330.0f, 90.0f };
	const float QUAD_ANGLES[] = { 45.0f, 135.0f, 225.0f, 315.0f, 315.0f, 45.0f };
	const float PENTAGON_ANGLES[] = { 18.0f, 90.0f, 162.0f, 234.0f, 306.0f, 18.0f };
	const float HITBOX_ANGLES[] = { 135.0f, 315.0f };
}

struct Vertex_glm {
	glm::vec3 position;
	glm::vec3 color;
};

struct Shape {
	std::vector<Vertex_glm> vertices;
	std::vector<unsigned int> indices;
	std::pair<glm::vec3, glm::vec3> hit_box;
	glm::vec3 center;
	glm::vec3 movment_vector;
	GLuint draw_mode;
};
struct ShapeData {
	std::vector<Vertex_glm> all_vertices;
	std::vector<unsigned int> all_indices;
	//size_t lines_index_count = 0;
	//size_t fan_index_count = 0;
};

struct ShapeManager {
	std::vector<Shape> all_shapes;
	ShapeData shape_data;

	void PrepareShapeData();
	void clear();
};

extern GLint Window_width, Window_height;

extern float shape_range, moving_speed;
extern int selected_shape_index, drag_dropped_shape_index;
extern int CreateShapeNum;

extern bool TransformSwitch;

GLvoid drawScene();
GLvoid Reshape(int w, int h);

void KeyBoard(unsigned char key, int x, int y);
void MouseClick(int button, int state, int x, int y);
void MouseMotion(int x, int y);
std::pair<float, float> ConvertScreenToOpenGL(int screen_x, int screen_y);

void INIT_BUFFER();
void UPDATE_BUFFER();

char* filetobuf(const char* file);
GLuint make_shaderProgram(const char* vertPath, const char* fragPath);

void SetupGeometry();
void SetupGeometry(glm::vec3 origin, int shape_type);
int PickObject(int x, int y);
void MergeShapes(int src_shape_index, int dst_shape_index);
void Transform();