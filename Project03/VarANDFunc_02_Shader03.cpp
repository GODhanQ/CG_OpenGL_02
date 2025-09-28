#include "VarANDFunc_02_Shader03.h"

extern GLint Window_width{ 1000 }, Window_height{ 1000 };
extern GLenum drawing_type{ NULL };

float Triangle_range_Min{ 0.1f }, Triangle_range_Max{ 0.2f };

bool moving_mode1{ false }, moving_mode2{ false }, moving_mode3{ false }, moving_mode4{ false }, is_activated_movement_func{ false };
glm::vec3 translation_vector1{ 0.0f, 0.0f, 0.0f },
	translation_vector2{ 0.0f, 0.0f, 0.0f },
	translation_vector3{ 0.0f, 0.0f, 0.0f },
	translation_vector4{ 0.0f, 0.0f, 0.0f };
glm::vec3 move_vector[4] = {
	{ 0.0f, 0.0f, 0.0f },
	{ 0.0f, 0.0f, 0.0f },
	{ 0.0f, 0.0f, 0.0f },
	{ 0.0f, 0.0f, 0.0f }
};
float model_rotation_angles[4] = { 0.0f };

ZigzagState zigzag_state[4] = { ZigzagState::HORIZONTAL };
int zigzag_vertical_direction[4] = { -1 };
int zigzag_transition_counter[4] = { 0 };
float zigzag_movement_speed[4] = { 0.0f };

glm::vec4 RectSpiral_boundary[4] = {
	// 0 : top boundary, 1 : right boundary, 2 : bottom boundary, 3 : left boundary
	{1.0f, 1.0f, -1.0f, -1.0f},
	{1.0f, 1.0f, -1.0f, -1.0f},
	{1.0f, 1.0f, -1.0f, -1.0f},
	{1.0f, 1.0f, -1.0f, -1.0f}
};
int rectspiral_direction[4]{ 0, 0, 0, 0 };
float RectSpiral_shrink_rate{ 0.3f };
bool is_centered[4]{ false, false, false, false };

float circle_spiral_angle[4]{ 0.0f, 0.0f, 0.0f, 0.0f };
float circle_spiral_radius[4]{ 0.0f, 0.0f, 0.0f, 0.0f };
float circle_spiral_angle_offset[4]{ 0.0f, 0.0f, 0.0f, 0.0f }; // 추가
bool is_shrinking[4]{ true, true, true, true };
glm::vec3 circle_spiral_center_offset[4];
float angular_speeds[4]{ 0.0f, 0.0f, 0.0f, 0.0f };
const float radius_change_rate = 0.001f;



void DrawBatchManager::prepareDrawCalls(const Shape* all_models) {
	fill_triangles_batch.counts.clear();
	fill_triangles_batch.indices_offsets.clear();
	fill_triangles_batch.basevertices.clear();
	fill_triangles_batch.draw_count = 0;

	line_triangles_batch.counts.clear();
	line_triangles_batch.indices_offsets.clear();
	line_triangles_batch.basevertices.clear();
	line_triangles_batch.draw_count = 0;

	for (int i = 0; i < 4; ++i) {
		const Shape& model = all_models[i];
		DrawCallParameters* selected_batch{ nullptr };

		if (model.is_active == false) {
			continue;
		}

		if (model.polygon_mode == GL_FILL) {
			selected_batch = &fill_triangles_batch;
		}
		else if (model.polygon_mode == GL_LINE) {
			selected_batch = &line_triangles_batch;
		}

		if (selected_batch) {
			selected_batch->counts.push_back(model.index_count);
			selected_batch->indices_offsets.push_back(model.index_offset);
			selected_batch->basevertices.push_back(model.base_vertex);
			selected_batch->draw_count++;
		}
	}
}

void DrawBatchManager::drawAll() {
	if (fill_triangles_batch.draw_count > 0) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		for (int i = 0; i < fill_triangles_batch.draw_count; ++i) {
			glDrawElements(
				GL_TRIANGLES,
				fill_triangles_batch.counts[i],
				GL_UNSIGNED_INT,
				(const void*)(fill_triangles_batch.indices_offsets[i])
			);
		}
	}

	if (line_triangles_batch.draw_count > 0) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		for (int i = 0; i < line_triangles_batch.draw_count; ++i) {
			glDrawElements(
				GL_TRIANGLES,
				line_triangles_batch.counts[i],
				GL_UNSIGNED_INT,
				(const void*)(line_triangles_batch.indices_offsets[i])
			);
		}
	}
}