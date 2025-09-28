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
#include <algorithm>
#include <map>
#define NOMINMAX
#include <windows.h>

struct Vertex_glm {
    glm::vec3 position;
    glm::vec3 color;
    int quadrant;
};

struct Shape {
    GLenum draw_mode;
    GLenum polygon_mode;
    GLsizei index_count;
    GLint base_vertex;
    size_t index_offset;
    GLint index_start;
    bool is_active;

    Shape() : draw_mode(GL_TRIANGLES), polygon_mode(0), index_count(0),
        base_vertex(0), index_offset(0), index_start(0), is_active(false) {}
};

class DrawBatchManager {
private:
    struct DrawCallParameters {
        std::vector<GLsizei> counts;
        std::vector<size_t> indices_offsets;
        std::vector<GLint> basevertices;
        GLsizei draw_count = 0;
    };

    DrawCallParameters fill_triangles_batch;
    DrawCallParameters line_triangles_batch;

public:
    void prepareDrawCalls(const Shape* all_models);
    void drawAll();
};

extern GLint Window_width, Window_height;
extern GLenum drawing_type;

extern float Triangle_range_Min, Triangle_range_Max;

char* filetobuf(const char* file);
void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();
GLvoid drawScene();
GLvoid Reshape(int w, int h);
void KeyBoard(unsigned char key, int x, int y);
void MouseClick(int button, int state, int x, int y);
void INIT_BUFFER();
void UPDATE_BUFFER();
std::pair<float, float> ConvertScreenToOpenGL(int screen_x, int screen_y);
void Make_Triangle(Shape& shape, float ogl_x, float ogl_y, int quardrant);