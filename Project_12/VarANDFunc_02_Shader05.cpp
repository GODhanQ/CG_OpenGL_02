#include "VarANDFunc_02_Shader05.h"

auto seed_1 = std::chrono::high_resolution_clock::now().time_since_epoch().count();
std::default_random_engine dre_1(seed_1);
std::uniform_real_distribution<float> urd_0_1_1(0.0f, 1.0f);

GLint Window_width{ 700 }, Window_height{ 700 };

bool is_all_drawing{ true };
//bool is_line2triangle{ false }, is_triangle2rectangle{ false }, is_rectangle2heptagon{ false }, is_heptagon2line{ false };
//bool is_transforming{ false };

// 0: line, 1: triangle, 2: rectangle, 3: heptagon, -1 : ignored
int current_shape_state{ -1 };

bool do_second_process{ false };

GLuint shared_ibo_line_id[2]{ 1, 2 }, shared_ibo_fan_id[7]{ 0, 1, 2, 3, 4, 5, 6 };
float radius_for_all{ 0.4f }, radius_for_one{ 0.7f };
float moving_speed_for_all{ 0.005f }, moving_speed_for_one{ 0.005f };

void ShapeManager::PrepareShapeData() {
	shape_data.positions.clear();
	shape_data.colors.clear();
	shape_data.centers.clear();
	shape_data.ver_mov_dirs.clear();
	shape_data.draw_modes.clear();
	shape_data.current_states.clear();
	shape_data.is_in_progress.clear();

	for (const auto& shape : all_shapes) {
		shape_data.positions.push_back(shape.center);
		shape_data.colors.push_back(shape.vertices[0].color);

		for (int i = 0; i < 6; ++i) {
			shape_data.positions.push_back(shape.vertices[i].position);
			shape_data.colors.push_back(shape.vertices[i].color);
			shape_data.centers.push_back(shape.center);
			shape_data.ver_mov_dirs.push_back(shape.ver_mov_dir[i]);
		}

		shape_data.draw_modes.push_back(shape.draw_mode);
		shape_data.current_states.push_back(shape.current_state);
		shape_data.is_in_progress.push_back(shape.is_in_progress);
	}
}


Shape::Shape() : center(0.0f), draw_mode(GL_LINES), is_in_progress(true), current_state(0) {
	glm::vec3 random_color = { urd_0_1_1(dre_1), urd_0_1_1(dre_1), urd_0_1_1(dre_1) };
	for (int i = 0; i < 6; i++) {
		vertices[i] = { center, random_color };
		ver_mov_dir[i] = glm::vec2(0.0f);
	}
}

Shape::Shape(glm::vec3 c) : center(c), draw_mode(GL_LINES), is_in_progress(true), current_state(0) {
	glm::vec3 random_color = { urd_0_1_1(dre_1), urd_0_1_1(dre_1), urd_0_1_1(dre_1) };
	for (int i = 0; i < 6; i++) {
		vertices[i] = { center, random_color };
		ver_mov_dir[i] = glm::vec2(0.0f);
	}
}

Shape::Shape(glm::vec3 c, int state) : center(c), is_in_progress(true), current_state(state) {
	if (current_state == 0) {
		draw_mode = GL_LINES;
		//draw_mode = GL_TRIANGLE_FAN;
	}
	else {
		draw_mode = GL_TRIANGLE_FAN;
	}
	glm::vec3 random_color = { urd_0_1_1(dre_1), urd_0_1_1(dre_1), urd_0_1_1(dre_1) };
	for (int i = 0; i < 6; i++) {
		vertices[i] = { center, random_color };
		ver_mov_dir[i] = glm::vec2(0.0f);
	}
}