#include "VarANDFunc_02_Shader13.h"

GLint Window_width{ 700 }, Window_height{ 700 };

float shape_range{ 0.2f };

int selected_shape_index = -1; // 선택된 도형 인덱스 (-1은 선택 없음)

void ShapeManager::PrepareShapeData() {
    shape_data.all_vertices.clear();
    shape_data.all_indices.clear();
    for (const auto& shape : all_shapes) {
        shape_data.all_vertices.insert(shape_data.all_vertices.end(), shape.vertices.begin(), shape.vertices.end());
        shape_data.all_indices.insert(shape_data.all_indices.end(), shape.indices.begin(), shape.indices.end());
    }
}

void ShapeManager::clear() {
    all_shapes.clear();
    shape_data.all_vertices.clear();
    shape_data.all_indices.clear();
}