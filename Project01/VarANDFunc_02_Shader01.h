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
#define NOMINMAX
#include <windows.h>

struct Shape {
	GLenum draw_mode;                               // e.g., GL_TRIANGLES, GL_LINES, etc.
	GLsizei index_count;                            // Number of indices for this shape
	GLint base_vertex;                              // Starting vertex in the VBO
	GLsizei index_offset;                           // Byte offset in the IBO
};

struct Vertex_glm {
	glm::vec3 position;
	glm::vec3 color;
};

class DrawBatchManager {
private:
    struct DrawCallParameters {
        std::vector<GLsizei> counts;                // 각 모델의 인덱스 개수
        std::vector<const void*> indices_offsets;   // IBO 내 바이트 오프셋 포인터
        std::vector<GLint> basevertices;            // VBO 내 기본 정점 인덱스
        GLsizei draw_count = 0;                     // 이 배치의 총 드로우 횟수 (counts.size())
    };

    // 2. 드로우 타입(Mode)별로 배치 파라미터를 분리하여 저장
	DrawCallParameters points_batch;
    DrawCallParameters lines_batch;
    DrawCallParameters triangles_batch;
	DrawCallParameters squares_batch;

public:
    // 초기화 및 업데이트 함수
    void prepareDrawCalls(const std::vector<Shape>& all_models);

    // 렌더링 함수
    void drawAll();
};

extern GLint Window_width, Window_height;

extern float vPositionList[];
extern float vColorList[];
extern unsigned int index[];
extern float Triangle_range;

extern bool DrawPoint_mode, DrawLine_mode, DrawTriangle_mode, DrawSquare_mode;
extern int Current_Diagram_Count, Selected_Diagram;
extern int active_line_strip_model_index; // 현재 그리고 있는 라인 스트립의 인덱스

extern int active_line_strip_model_index;

extern bool is_picking_mode; // 객체 선택 모드 활성화 상태
extern int selected_model_index; // 선택된 객체의 인덱스

extern glm::vec3 previous_movement_vec;
extern glm::vec3 movement_vec;
extern float movement_speed;
extern bool moving_flag;

char* filetobuf(const char* file);
void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();
int PickObject(int x, int y);
GLvoid drawScene();
GLvoid Reshape(int w, int h);
void Keyboard(unsigned char key, int x, int y);
void SpecialKeyboard(int key, int x, int y);
void SpecialKeyboardUp(int key, int x, int y);
void MouseClick(int button, int state, int x, int y);
void INIT_BUFFER();
void UPDATE_BUFFER();
std::pair<float, float> ConvertMouseWxy2GLxy(int x, int y);
void CreatePointAtOrigin(float ogl_x, float ogl_y);
void AddVertexToLineStrip(float ogl_x, float ogl_y); // 함수 이름 변경
void CreateTriangleAtOrigin(float ogl_x, float ogl_y);
void CreateSquareAtOrigin(float ogl_x, float ogl_y);
void Translation();
void ChangeConsoleColor(int color);