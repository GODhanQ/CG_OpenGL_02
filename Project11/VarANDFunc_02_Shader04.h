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
    glm::vec3 initial_center;
    glm::vec3 color;
    float state;

    float expand_angle;
    float expand_radius;

    glm::vec3 stop_position; 
    glm::vec3 shrink_center;
    float shrink_progress;
    float shrink_initial_radius;
};

struct Spiral {
    glm::vec3 center;
    glm::vec3 color;
    bool generation_complete;
    std::vector<Vertex_glm> vertices;
};

extern GLint Window_width, Window_height;

extern int Rotation_Num, Spin_Speed;
extern float Radius_change_Speed;
extern float Max_Angle;
extern float time_by_sec;
extern float rotate_speed;

extern GLfloat bgColorR, bgColorG, bgColorB;
extern GLenum drawing_type;


GLvoid drawScene();
GLvoid Reshape(int w, int h);

void KeyBoard(unsigned char key, int x, int y);
void MouseClick(int button, int state, int x, int y);
std::pair<float, float> ConvertScreenToOpenGL(int screen_x, int screen_y);

void INIT_BUFFER();
void UPDATE_BUFFER(const std::vector<Vertex_glm>& all_vertices, const std::vector<unsigned int>& all_indices);

char* filetobuf(const char* file);
void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();

void CreateSpiralAt(glm::vec3 center);