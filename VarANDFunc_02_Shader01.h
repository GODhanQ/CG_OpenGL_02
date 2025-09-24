#pragma once
#include <vector>
#include <chrono>
#include <random>
#include <GL/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <glm/glm.hpp>

// 정점 데이터 구조체
struct Vertex_glm {
	glm::vec3 position;
	glm::vec3 color;
};

// 전역 변수 선언
extern GLint Window_width;
extern GLint Window_height;
extern GLfloat Triangle_range;

// 올바른 자료구조 사용
extern std::vector<Vertex_glm> vertices;
extern std::vector<unsigned int> indices;

// 함수 프로토타입
void main(int argc, char** argv);
GLvoid drawScene();
GLvoid Reshape(int w, int h);
void Keyboard(unsigned char key, int x, int y);
void MouseClick(int button, int state, int x, int y);
std::pair<float, float> ConvertMouseWxy2GLxy(int x, int y);
char* filetobuf(const char* file);
void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();
void INIT_BUFFER();
void UPDATE_BUFFER();