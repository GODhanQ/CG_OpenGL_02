#ifndef VARANDFUNC_02_SHADER01_H
#endif VARANDFUNC_02_SHADER01_H

#define _CRT_SECURE_NO_WARNINGS
#include "VarANDFunc_02_Shader01.h"

GLint Window_width{ 1000 }, Window_height{ 1000 };
float Triangle_range{ 0.05f };

bool DrawPoint_mode{ false }, DrawLine_mode{ false }, DrawTriangle_mode{ false }, DrawSquare_mode{ false };
int Current_Diagram_Count{}, Selected_Diagram{ -1 };

int active_line_strip_model_index = -1;

bool is_picking_mode = false; // 객체 선택 모드 활성화 상태
int selected_model_index = -1; // 선택된 객체의 인덱스

glm::vec3 previous_movement_vec{ 0.0f, 0.0f, 0.0f };
glm::vec3 movement_vec{ 0, 0, 0 };
float movement_speed{ 0.002f };
bool moving_flag{ false };

struct Vertex {
	std::tuple<float, float, float> position;
	std::tuple<float, float, float> color;
};

float vPositionList_const[] = {
	0.5f, 0.5f, 0.0f, // 우측 상단
	0.5f, -0.5f, 0.0f, // 우측 하단
	-0.5f, -0.5f, 0.0f, // 좌측 하단
	-0.5f, 0.5f, 0.0f // 좌측 상단
};
float vColorList_const[] = {
	1.0f, 0.0f, 0.0f, // 우측 상단	R
	0.0f, 1.0f, 0.0f, // 우측 하단	G
	0.0f, 0.0f, 1.0f, // 좌측 하단	B
	1.0f, 1.0f, 0.0f // 좌측 상단	Y
};

unsigned int index[] = {
	0, 1, 3, // 첫 번째 삼각형
	1, 2, 3 // 두 번째 삼각형
	//0, 1, 2, 3 // 사각형
};

void DrawBatchManager::prepareDrawCalls(const std::vector<Shape>& all_models) {
	points_batch = {};
    lines_batch = {};
    triangles_batch = {};
	squares_batch = {};

    for (const auto& model : all_models) {
        DrawCallParameters* selected_batch = nullptr;

        if (model.draw_mode == GL_POINTS) {
            selected_batch = &points_batch;
		}
        else if (model.draw_mode == GL_LINE_STRIP) {
            selected_batch = &lines_batch;
        }
        else if (model.draw_mode == GL_TRIANGLES) {
            selected_batch = &triangles_batch;
        }
        else if (model.draw_mode == GL_QUADS) {
            selected_batch = &squares_batch;
		}
        else {
			std::cout << "Not supported draw mode encountered. Skipping model.\n";
            continue;
        }

        selected_batch->counts.push_back(model.index_count);
        selected_batch->indices_offsets.push_back((const void*)(intptr_t)model.index_offset);
        selected_batch->basevertices.push_back(model.base_vertex);
    }

    // 드로우 횟수 업데이트
	points_batch.draw_count = points_batch.counts.size();
    lines_batch.draw_count = lines_batch.counts.size();
    triangles_batch.draw_count = triangles_batch.counts.size();
	squares_batch.draw_count = squares_batch.counts.size();
}

void DrawBatchManager::drawAll() {
    if (points_batch.draw_count > 0) {
        glMultiDrawElementsBaseVertex(
            GL_POINTS,
            points_batch.counts.data(),
            GL_UNSIGNED_INT,
            const_cast<void**>(reinterpret_cast<const void* const*>(points_batch.indices_offsets.data())),
            points_batch.draw_count,
            points_batch.basevertices.data()
        );
    }
    if (lines_batch.draw_count > 0) {
        glMultiDrawElementsBaseVertex(
            GL_LINE_STRIP,
            lines_batch.counts.data(),
            GL_UNSIGNED_INT,
            const_cast<void**>(reinterpret_cast<const void* const*>(lines_batch.indices_offsets.data())),
            lines_batch.draw_count,
            lines_batch.basevertices.data()
        );
    }
    if (triangles_batch.draw_count > 0) {
        glMultiDrawElementsBaseVertex(
            GL_TRIANGLES,
            triangles_batch.counts.data(),
            GL_UNSIGNED_INT,
            const_cast<void**>(reinterpret_cast<const void* const*>(triangles_batch.indices_offsets.data())),
            triangles_batch.draw_count,
            triangles_batch.basevertices.data()
        );
    }
    if (squares_batch.draw_count > 0) {
        glMultiDrawElementsBaseVertex(
            GL_QUADS,
            squares_batch.counts.data(),
            GL_UNSIGNED_INT,
            const_cast<void**>(reinterpret_cast<const void* const*>(squares_batch.indices_offsets.data())),
            squares_batch.draw_count,
            squares_batch.basevertices.data()
        );
    }
}