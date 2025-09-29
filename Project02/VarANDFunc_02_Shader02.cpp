#include "VarANDFunc_02_Shader02.h"

extern GLint Window_width{ 1000 }, Window_height{ 1000 };
extern GLenum drawing_type{ NULL };

float Triangle_range_Min{ 0.05f }, Triangle_range_Max{ 0.1f };

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

void DrawAxisBatchManager::prepareDrawCalls(const Shape* all_models) {
	axis_lines_batch.counts.clear();
	axis_lines_batch.indices_offsets.clear();
	axis_lines_batch.basevertices.clear();
	axis_lines_batch.draw_count = 0;
	for (int i = 0; i < 4; ++i) {
		const Shape& model = all_models[i];
		if (model.is_active == false) {
			continue;
		}
		if (model.polygon_mode == GL_LINE) {
			axis_lines_batch.counts.push_back(model.index_count);
			axis_lines_batch.indices_offsets.push_back(model.index_offset);
			axis_lines_batch.basevertices.push_back(model.base_vertex);
			axis_lines_batch.draw_count++;
		}
	}
}
void DrawAxisBatchManager::drawAll() {
	if (axis_lines_batch.draw_count > 0) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glLineWidth(3.0f);
		for (int i = 0; i < axis_lines_batch.draw_count; ++i) {
			glDrawElements(
				GL_LINES,
				axis_lines_batch.counts[i],
				GL_UNSIGNED_INT,
				(const void*)(axis_lines_batch.indices_offsets[i])
			);
		}
	}
	glLineWidth(1.0f);
}