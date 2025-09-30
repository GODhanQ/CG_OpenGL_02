 #define _CRT_SECURE_NO_WARNINGS
#include "VarANDFunc_02_Shader03.h"

auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
std::default_random_engine dre(seed);
std::uniform_real_distribution<float> urd_0_1(0.0f, 1.0f);
std::uniform_real_distribution<float> urd_m1_1(-1.0f, 1.0f);
std::uniform_real_distribution<float> urdRange(Triangle_range_Min, Triangle_range_Max);
std::uniform_real_distribution<float> urd_mov_vec(0.005f, 0.015f);

GLuint shaderProgramID;
GLuint vertexShader;
GLuint fragmentShader;

// Index 0 : 1 qaudrant | Index 1 : 2 qaudrant | Index 2 : 3 qaudrant | Index 3 : 4 qaudrant
Shape models[4];

GLuint VAO, VBO, EBO;
DrawBatchManager drawBatchManager;

std::vector<Vertex_glm> Vertex_glm_vec;
std::vector<unsigned int> index_vec;

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | 0x0000);
	glutInitWindowPosition(500, 10);
	glutInitWindowSize(Window_width, Window_height);
	glutCreateWindow("Example2");

	glewExperimental = GL_TRUE;
	glewInit();

	make_vertexShaders();
	make_fragmentShaders();
	shaderProgramID = make_shaderProgram();

	INIT_BUFFER();

	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(KeyBoard);
	glutMouseFunc(MouseClick);

	glutMainLoop();
}

GLvoid drawScene() {
	GLfloat rColor{ 0.3f }, gColor{ 0.3f }, bColor{ 0.3f };
	glClearColor(rColor, gColor, bColor, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(shaderProgramID);

	if (moving_mode1 || moving_mode2 || moving_mode3 || moving_mode4) {
		Translation();
	}

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	drawBatchManager.prepareDrawCalls(models);
	drawBatchManager.drawAll();

	glBindVertexArray(0);

	/*std::cout << "============ CURRENT STATE ============\n";
	for (int i = 0; i < 4; ++i) {
		if (models[i].is_active) {
			std::cout << "Model " << i << " - Mode: "
				<< (models[i].polygon_mode == GL_FILL ? "FILL" : "LINE")
				<< ", Base Vertex: " << models[i].base_vertex
				<< ", Index Offset: " << models[i].index_offset
				<< ", Index Count: " << models[i].index_count << "\n";
		}
	}
	std::cout << "============ STATE END ============\n";
	std::cout << "============ INDEX & VERTEX LIST ============\n";
	for (size_t i = 0; i < index_vec.size(); i += 3) {
		if (i + 2 < index_vec.size()) {
			std::cout << "(" << index_vec[i] << ", " << index_vec[i + 1] << ", " << index_vec[i + 2] << ")\n";
		}
	}
	for (size_t i = 0; i < Vertex_glm_vec.size(); ++i) {
		const auto& v = Vertex_glm_vec[i];
		std::cout << "Vertex " << i << " - Pos: (" << v.position.x << ", " << v.position.y << ", " << v.position.z
			<< "), Color: (" << v.color.r << ", " << v.color.g << ", " << v.color.b
			<< "), Quadrant: " << v.quadrant << "\n";
	}
	std::cout << "============ INDEX & VERTEX END ============\n";*/

	glutSwapBuffers();
}
GLvoid Reshape(int w, int h) {
	glViewport(0, 0, w, h);
}

void KeyBoard(unsigned char key, int x, int y) {
	switch (key) {
	case'1':
		if (!moving_mode1 && (moving_mode2 || moving_mode3 || moving_mode4)) {
			std::cout << "Another movement mode is already active. Please deactivate it first.\n";
			return;
		}
		moving_mode1 = !moving_mode1;
		is_activated_movement_func = true;

		if (moving_mode1) {
			for (int i = 0; i < 4; ++i) {
				move_vector[i] = { urd_mov_vec(dre), urd_mov_vec(dre), 0.0f };
			}
		}
		std::cout << "Movement Mode 1 (Bouncing) : " << (moving_mode1 ? "ON" : "OFF") << "\n";
		glutPostRedisplay();
		break;
	case'2':
		if (!moving_mode2 && (moving_mode1 || moving_mode3 || moving_mode4)) {
			std::cout << "Another movement mode is already active. Please deactivate it first.\n";
			return;
		}
		moving_mode2 = !moving_mode2;
		is_activated_movement_func = true;

		if (moving_mode2) {
			for (int i = 0; i < 4; ++i) {
				zigzag_movement_speed[i] = urd_mov_vec(dre);
				move_vector[i] = { -zigzag_movement_speed[i], 0.0f, 0.0f }; // Start moving left
				zigzag_state[i] = ZigzagState::HORIZONTAL;
				zigzag_vertical_direction[i] = -1; // Start by moving down
				zigzag_transition_counter[i] = 0;
			}
		}

		std::cout << "Movement Mode 2 (Zig Zag) : " << (moving_mode2 ? "ON" : "OFF") << "\n";
		glutPostRedisplay();
		break;
	case'3':
		if (!moving_mode3 && (moving_mode1 || moving_mode2 || moving_mode4)) {
			std::cout << "Another movement mode is already active. Please deactivate it first.\n";
			return;
		}
		moving_mode3 = !moving_mode3;
		is_activated_movement_func = true;

		if (moving_mode3) {
			for (int i = 0; i < 4; ++i) {
				float speed = urd_mov_vec(dre);
				move_vector[i] = { speed, 0.0f, 0.0f }; // Start moving right
				rectspiral_direction[i] = 2; // 2: right
				RectSpiral_boundary[i] = { 1.0f, 1.0f, -1.0f, -1.0f };
				is_centered[i] = false; // Reset centered flag
			}
		}
		std::cout << "Movement Mode 3 (Rectangular Spiral) : " << (moving_mode3 ? "ON" : "OFF") << "\n";
		glutPostRedisplay();
		break;
	case'4':
		if (!moving_mode4 && (moving_mode1 || moving_mode2 || moving_mode3)) {
			std::cout << "Another movement mode is already active. Please deactivate it first.\n";
			return;
		}
		moving_mode4 = !moving_mode4;
		is_activated_movement_func = moving_mode4;

		if (moving_mode4) {
			for (int i = 0; i < 4; ++i) {
				// 각 객체에 독립적인 회전 속도 할당
				angular_speeds[i] = urd_mov_vec(dre);
				// 각 사분면에 따라 90도씩 차이를 두어 각도 오프셋 설정
				circle_spiral_angle_offset[i] = glm::radians(90.0f * i);
				circle_spiral_angle[i] = 0.0f;
				circle_spiral_radius[i] = 0.7f; // 시작 반지름
				is_shrinking[i] = true;
			}
		}
		std::cout << "Movement Mode 4 (Circle Spiral) : " << (moving_mode4 ? "ON" : "OFF") << "\n";
		glutPostRedisplay();
		break;
	case 'a':
		std::cout << "Fill Mode Selected\n";
		drawing_type = GL_FILL;
		break;
	case 'b':
		std::cout << "Line Mode Selected\n";
		drawing_type = GL_LINE;
		break;
	case 'c':
		std::cout << "Clearing all triangles\n";
		clearModels();
		break;
	case 'q':
		exit(0);
	}
}
void MouseClick(int button, int state, int x, int y) {
	if (is_activated_movement_func) {
		std::cout << "Movement mode was activated. Cannot modify triangles.\n";
		return;
	}
	if (drawing_type == NULL) {
		std::cout << "Please select a drawing mode first ( 'a' : Fill, 'b' : Line )\n";
		return;
	}

	if (state == GLUT_DOWN) {
		std::pair<float, float> ogl_xy = ConvertScreenToOpenGL(x, y);
		float ogl_x = ogl_xy.first, ogl_y = ogl_xy.second;
		std::cout << "Mouse Clicked at ( " << ogl_x << ", " << ogl_y << " )\n";

		int target_quadrant = -1;
		if (ogl_x >= 0.0f && ogl_y >= 0.0f) target_quadrant = 0;
		else if (ogl_x < 0.0f && ogl_y >= 0.0f) target_quadrant = 1;
		else if (ogl_x < 0.0f && ogl_y < 0.0f) target_quadrant = 2;
		else if (ogl_x >= 0.0f && ogl_y < 0.0f) target_quadrant = 3;

		if (target_quadrant == -1) {
			std::cout << "Error determining quadrant.\n";
			return;
		}

		// 1. 기존 상태 저장 (폴리곤 모드, 우클릭 시 필요한 색상)
		std::map<int, GLenum> old_polygon_modes;
		std::vector<glm::vec3> old_colors;
		for (int i = 0; i < 4; ++i) {
			if (models[i].is_active) {
				old_polygon_modes[i] = models[i].polygon_mode;
				if (button == GLUT_RIGHT_BUTTON && i == target_quadrant) {
					for (int j = 0; j < models[i].index_count; ++j) {
						unsigned int vertex_index = index_vec[models[i].index_start + j];
						if (vertex_index < Vertex_glm_vec.size()) {
							old_colors.push_back(Vertex_glm_vec[vertex_index].color);
						}
					}
				}
			}
		}

		// 2. 새 정점 목록 생성
		std::vector<Vertex_glm> new_vertex_vec;
		// 대상이 아닌 사분면의 정점들은 그대로 복사
		for (const auto& v : Vertex_glm_vec) {
			if (v.quadrant != target_quadrant) {
				new_vertex_vec.push_back(v);
			}
		}

		// 대상 사분면에 새 삼각형 정점 추가
		glm::vec3 origin{ ogl_x, ogl_y, 0.0f };
		Vertex_glm v1, v2, v3;
		float top_ver_range = urdRange(dre);
		float left_bottom_range = urdRange(dre);
		float right_bottom_range = urdRange(dre);
		v1.position = origin + glm::vec3(0.0f, top_ver_range, 0.0f);
		v2.position = origin + glm::vec3(-left_bottom_range, -left_bottom_range, 0.0f);
		v3.position = origin + glm::vec3(right_bottom_range, -right_bottom_range, 0.0f);
		
		if (button == GLUT_LEFT_BUTTON) { // 좌클릭: 모양과 색상 모두 변경 (새 랜덤 색상)
			v1.color = glm::vec3(urd_0_1(dre), urd_0_1(dre), urd_0_1(dre));
			v2.color = glm::vec3(urd_0_1(dre), urd_0_1(dre), urd_0_1(dre));
			v3.color = glm::vec3(urd_0_1(dre), urd_0_1(dre), urd_0_1(dre));
		} else { // 우클릭: 모양만 변경 (기존 색상 유지)
			if (old_colors.size() >= 3) {
				v1.color = old_colors[0];
				v2.color = old_colors[1];
				v3.color = old_colors[2];
			} else { // 기존 색상이 없으면 랜덤 색상으로 대체
				v1.color = glm::vec3(urd_0_1(dre), urd_0_1(dre), urd_0_1(dre));
				v2.color = glm::vec3(urd_0_1(dre), urd_0_1(dre), urd_0_1(dre));
				v3.color = glm::vec3(urd_0_1(dre), urd_0_1(dre), urd_0_1(dre));
			}
		}
		v1.quadrant = v2.quadrant = v3.quadrant = target_quadrant;
		new_vertex_vec.push_back(v1);
		new_vertex_vec.push_back(v2);
		new_vertex_vec.push_back(v3);

		// 3. 정점 데이터 정렬 및 교체
		std::sort(new_vertex_vec.begin(), new_vertex_vec.end(), [](const Vertex_glm& a, const Vertex_glm& b) {
			return a.quadrant < b.quadrant;
		});
		Vertex_glm_vec = new_vertex_vec;

		// 4. 인덱스 및 모델 정보 완전 재구성
		index_vec.clear();
		for (int i = 0; i < 4; ++i) {
			models[i].is_active = false;
			std::vector<unsigned int> quadrant_indices;
			for (unsigned int j = 0; j < Vertex_glm_vec.size(); ++j) {
				if (Vertex_glm_vec[j].quadrant == i) {
					quadrant_indices.push_back(j);
				}
			}

			if (!quadrant_indices.empty()) {
				models[i].is_active = true;
				if (i == target_quadrant) {
					models[i].polygon_mode = drawing_type;
				} else if (old_polygon_modes.count(i)) {
					models[i].polygon_mode = old_polygon_modes[i];
				} else {
					models[i].polygon_mode = GL_FILL; // 기본값
				}

				models[i].base_vertex = quadrant_indices.empty() ? 0 : quadrant_indices[0];
				models[i].index_start = index_vec.size();
				models[i].index_count = quadrant_indices.size();
				models[i].index_offset = models[i].index_start * sizeof(unsigned int);
				
				index_vec.insert(index_vec.end(), quadrant_indices.begin(), quadrant_indices.end());
			}
		}

		drawBatchManager.prepareDrawCalls(models);
		UPDATE_BUFFER();
		glutPostRedisplay();
	}
}
std::pair<float, float> ConvertScreenToOpenGL(int screen_x, int screen_y) {
	int width = glutGet(GLUT_WINDOW_WIDTH);
	int height = glutGet(GLUT_WINDOW_HEIGHT);

	float ogl_x = (2.0f * screen_x) / width - 1.0f;
	float ogl_y = 1.0f - (2.0f * screen_y) / height;

	return { ogl_x, ogl_y };
}

void INIT_BUFFER()
{
	std::cout << "Initializing VAO, EBO \n";
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_glm), (void*)offsetof(Vertex_glm, position));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_glm), (void*)offsetof(Vertex_glm, color));
	glEnableVertexAttribArray(1);
	glVertexAttribIPointer(2, 1, GL_INT, sizeof(Vertex_glm), (void*)offsetof(Vertex_glm, quadrant));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
void UPDATE_BUFFER()
{
	if (Vertex_glm_vec.empty() && index_vec.empty()) {
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		return;
	}
	//std::cout << "Updating VBO, EBO \n";

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, Vertex_glm_vec.size() * sizeof(Vertex_glm), Vertex_glm_vec.data(), GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_vec.size() * sizeof(unsigned int), index_vec.data(), GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	//std::cout << "Buffer Update Completed\n";
}

void Make_Triangle(Shape& shape, float ogl_x, float ogl_y, int quardrant) {
	glm::vec3 origin{ ogl_x, ogl_y, 0.0f };
	Vertex_glm v1, v2, v3;

	float triangle_range = urdRange(dre);

	v1.position = origin + glm::vec3(0.0f, triangle_range, 0.0f);
	v2.position = origin + glm::vec3(-triangle_range, -triangle_range, 0.0f);
	v3.position = origin + glm::vec3(triangle_range, -triangle_range, 0.0f);

	v1.color = glm::vec3(urd_0_1(dre), urd_0_1(dre), urd_0_1(dre));
	v2.color = glm::vec3(urd_0_1(dre), urd_0_1(dre), urd_0_1(dre));
	v3.color = glm::vec3(urd_0_1(dre), urd_0_1(dre), urd_0_1(dre));

	v1.quadrant = quardrant; v2.quadrant = quardrant; v3.quadrant = quardrant;

	std::cout << "Triangle vertex positions:\n";
	std::cout << "  v1: (" << v1.position.x << ", " << v1.position.y << ", " << v1.position.z << ")\n";
	std::cout << "  v2: (" << v2.position.x << ", " << v2.position.y << ", " << v2.position.z << ")\n";
	std::cout << "  v3: (" << v3.position.x << ", " << v3.position.y << ", " << v3.position.z << ")\n";

	shape.base_vertex = static_cast<GLint>(Vertex_glm_vec.size());
	shape.index_offset = static_cast<GLsizei>(index_vec.size() * sizeof(unsigned int));

	std::cout << "Shape info - base_vertex: " << shape.base_vertex
		<< ", index_offset: " << shape.index_offset << "\n";

	Vertex_glm_vec.push_back(v1);
	Vertex_glm_vec.push_back(v2);
	Vertex_glm_vec.push_back(v3);

	unsigned int base_index = static_cast<unsigned int>(shape.base_vertex);
	shape.index_start = index_vec.size();
	index_vec.push_back(base_index);
	index_vec.push_back(base_index + 1);
	index_vec.push_back(base_index + 2);

	std::cout << "Index values: " << base_index << ", " << (base_index + 1)
		<< ", " << (base_index + 2) << "\n";

	shape.index_count = 3;
	shape.polygon_mode = drawing_type;
	shape.is_active = true;
}
void Translation() {
	if (moving_mode1) {
		BouncingMovement();
	}
	else if (moving_mode2) {
		ZigzagMovement();
	}
	else if (moving_mode3) {
		RectSpiralMovement();
	}
	else if (moving_mode4) {
		CircleSpiralMovement();
	}

	UPDATE_BUFFER();
	glutPostRedisplay();
}
void clearModels() {
	is_activated_movement_func = false;
	moving_mode1 = moving_mode2 = moving_mode3 = moving_mode4 = false;

	for (int i = 0; i < 4; i++) {
		models[i] = Shape();
		rectspiral_direction[i] = 0;
		RectSpiral_boundary[i] = { 1.0f, 1.0f, -1.0f, -1.0f };
		is_centered[i] = false;
		circle_spiral_angle[i] = 0.0f;
		circle_spiral_radius[i] = 0.0f;
		is_shrinking[i] = true;
		circle_spiral_angle_offset[i] = 0.0f;
		angular_speeds[i] = 0.0f;
		model_rotation_angles[i] = 0.0f;

		// Zigzag 상태 초기화
		zigzag_state[i] = ZigzagState::HORIZONTAL;
		zigzag_vertical_direction[i] = -1;
		zigzag_transition_counter[i] = 0;
		zigzag_movement_speed[i] = 0.0f;
	}
	Vertex_glm_vec.clear();
	index_vec.clear();

	drawBatchManager.prepareDrawCalls(models);
	UPDATE_BUFFER();
	glutPostRedisplay();
}
void ApplyRotation(int model_index, float angle_degrees) {
	if (!models[model_index].is_active || models[model_index].index_count == 0) {
		return;
	}

	// 1. 모델의 중심 계산
	glm::vec3 center(0.0f);
	for (int j = 0; j < models[model_index].index_count; ++j) {
		unsigned int vertex_index = index_vec[models[model_index].index_start + j];
		center += Vertex_glm_vec[vertex_index].position;
	}
	center /= models[model_index].index_count;

	// 2. 회전 각도 계산
	float current_angle_rad = glm::radians(model_rotation_angles[model_index]);
	float target_angle_rad = glm::radians(angle_degrees);
	float rotation_angle_rad = target_angle_rad - current_angle_rad;

	// 3. 회전 행렬 생성
	glm::mat4 rotation_matrix = glm::rotate(glm::mat4(1.0f), rotation_angle_rad, glm::vec3(0.0f, 0.0f, 1.0f));

	// 4. 각 정점을 중심 기준으로 회전
	for (int j = 0; j < models[model_index].index_count; ++j) {
		unsigned int vertex_index = index_vec[models[model_index].index_start + j];
		Vertex_glm& vertex = Vertex_glm_vec[vertex_index];

		// 중심으로 이동 -> 회전 -> 원래 위치로 복귀
		glm::vec4 temp_pos = glm::vec4(vertex.position - center, 1.0f);
		temp_pos = rotation_matrix * temp_pos;
		vertex.position = glm::vec3(temp_pos) + center;
	}

	// 5. 현재 회전 각도 업데이트
	model_rotation_angles[model_index] = angle_degrees;
}

void BouncingMovement() {
	for (int i = 0; i < 4; ++i) {
		if (models[i].is_active) {
			bool collision = false;
			for (int j = models[i].index_start; j < models[i].index_start + models[i].index_count; ++j) {
				unsigned int vertex_index = index_vec[j];
				Vertex_glm& vertex = Vertex_glm_vec[vertex_index];
				if (vertex.position.x + move_vector[i].x > 1.0f) {
					move_vector[i].x = -move_vector[i].x;
					ApplyRotation(i, 90.0f);
					collision = true; break;
				}
				else if (vertex.position.x + move_vector[i].x < -1.0f) {
					move_vector[i].x = -move_vector[i].x;
					ApplyRotation(i, 270.0f);
					collision = true; break;
				}
				if (vertex.position.y + move_vector[i].y > 1.0f) {
					move_vector[i].y = -move_vector[i].y;
					ApplyRotation(i, 180.0f);
					collision = true; break;
				}
				else if (vertex.position.y + move_vector[i].y < -1.0f) {
					move_vector[i].y = -move_vector[i].y;
					ApplyRotation(i, 0.0f);
					collision = true; break;
				}
			}
			if (collision) {
				// 충돌 후 위치 보정
				for (int j = models[i].index_start; j < models[i].index_start + models[i].index_count; ++j) {
					unsigned int vertex_index = index_vec[j];
					Vertex_glm& vertex = Vertex_glm_vec[vertex_index];
					vertex.position.x = glm::clamp(vertex.position.x, -1.0f, 1.0f);
					vertex.position.y = glm::clamp(vertex.position.y, -1.0f, 1.0f);
				}
			}

			for (int j = models[i].index_start; j < models[i].index_start + models[i].index_count; ++j) {
				unsigned int vertex_index = index_vec[j];
				Vertex_glm& vertex = Vertex_glm_vec[vertex_index];
				vertex.position += move_vector[i];
			}
		}
	}
}
void ZigzagMovement() {
	const int transition_duration = 30; // 약 0.5초 (60FPS 기준)

	for (int i = 0; i < 4; ++i) {
		if (!models[i].is_active) continue;

		// 상하단 경계 충돌 시 수직 이동 방향 변경 및 회전
		for (int j = models[i].index_start; j < models[i].index_start + models[i].index_count; ++j) {
			unsigned int vertex_index = index_vec[j];
			Vertex_glm& vertex = Vertex_glm_vec[vertex_index];
			if (vertex.position.y >= 1.0f && zigzag_vertical_direction[i] > 0) {
				zigzag_vertical_direction[i] = -1; // 천장에 닿으면 아래로
				ApplyRotation(i, 0.0f); // 아래 방향으로 회전
			}
			else if (vertex.position.y <= -1.0f && zigzag_vertical_direction[i] < 0) {
				zigzag_vertical_direction[i] = 1; // 바닥에 닿으면 위로
				ApplyRotation(i, 180.0f); // 위 방향으로 회전
			}
		}

		if (zigzag_state[i] == ZigzagState::HORIZONTAL) {
			bool horizontal_collision = false;
			for (int j = models[i].index_start; j < models[i].index_start + models[i].index_count; ++j) {
				unsigned int vertex_index = index_vec[j];
				Vertex_glm& vertex = Vertex_glm_vec[vertex_index];
				// 다음 프레임의 위치를 예측하여 충돌 검사
				float next_x = vertex.position.x + move_vector[i].x;
				if ((next_x >= 1.0f && move_vector[i].x > 0) || (next_x <= -1.0f && move_vector[i].x < 0)) {
					horizontal_collision = true;
					break;
				}
			}

			if (horizontal_collision) {
				// 충돌 시 위치 보정
				float overshoot = 0.0f;
				if (move_vector[i].x > 0) { // 오른쪽 벽과 충돌
					float max_x = -2.0f;
					for (int j = models[i].index_start; j < models[i].index_start + models[i].index_count; ++j) {
						max_x = std::max(max_x, Vertex_glm_vec[index_vec[j]].position.x);
					}
					overshoot = max_x - 1.0f;
				}
				else { // 왼쪽 벽과 충돌
					float min_x = 2.0f;
					for (int j = models[i].index_start; j < models[i].index_start + models[i].index_count; ++j) {
						min_x = std::min(min_x, Vertex_glm_vec[index_vec[j]].position.x);
					}
					overshoot = min_x + 1.0f;
				}

				for (int j = models[i].index_start; j < models[i].index_start + models[i].index_count; ++j) {
					Vertex_glm_vec[index_vec[j]].position.x -= overshoot;
				}

				zigzag_state[i] = ZigzagState::VERTICAL_TRANSITION;
				zigzag_transition_counter[i] = transition_duration;
				// 수직 이동 시작
				move_vector[i].y = abs(zigzag_movement_speed[i]) * zigzag_vertical_direction[i];
				move_vector[i].x = 0;

				// 수직 이동 방향에 맞춰 회전
				if (zigzag_vertical_direction[i] > 0) {
					ApplyRotation(i, 180.0f); // 위
				}
				else {
					ApplyRotation(i, 0.0f); // 아래
				}
			}
		}
		else if (zigzag_state[i] == ZigzagState::VERTICAL_TRANSITION) {
			std::cout << "move vertical" << std::endl;
			zigzag_transition_counter[i]--;
			if (zigzag_transition_counter[i] <= 0) {
				zigzag_state[i] = ZigzagState::HORIZONTAL;
				// 수평 이동 방향 전환하여 다시 시작
				zigzag_movement_speed[i] = -zigzag_movement_speed[i];
				move_vector[i].x = zigzag_movement_speed[i];
				move_vector[i].y = 0;

				// 수평 이동 방향에 맞춰 회전
				if (move_vector[i].x > 0) {
					ApplyRotation(i, 90.0f); // 오른쪽
				}
				else {
					ApplyRotation(i, 270.0f); // 왼쪽
				}
			}
		}

		// 모든 정점 위치 업데이트
		for (int j = models[i].index_start; j < models[i].index_start + models[i].index_count; ++j) {
			unsigned int vertex_index = index_vec[j];
			Vertex_glm& vertex = Vertex_glm_vec[vertex_index];
			vertex.position += move_vector[i];
		}
	}
}
void RectSpiralMovement() {
	for (int i = 0; i < 4; ++i) {
		if (models[i].is_active) {
			// 나선형 움직임 종료 조건: 경계가 역전되었고, 아직 중앙 정렬이 안된 경우
			if (!is_centered[i] && (RectSpiral_boundary[i][3] >= RectSpiral_boundary[i][1] || RectSpiral_boundary[i][2] >= RectSpiral_boundary[i][0])) {
				move_vector[i] = { 0.0f, 0.0f, 0.0f };

				// 모델의 현재 중심 계산
				glm::vec3 current_center(0.0f);
				if (models[i].index_count > 0) {
					for (int j = 0; j < models[i].index_count; ++j) {
						unsigned int vertex_index = index_vec[models[i].index_start + j];
						current_center += Vertex_glm_vec[vertex_index].position;
					}
					current_center /= models[i].index_count;
				}

				// 중심을 원점(0,0,0)으로 이동시키는 변환 벡터 계산
				glm::vec3 translation_to_center = -current_center;

				// 모델의 모든 정점에 변환 적용
				for (int j = 0; j < models[i].index_count; ++j) {
					unsigned int vertex_index = index_vec[models[i].index_start + j];
					Vertex_glm_vec[vertex_index].position += translation_to_center;
				}
				is_centered[i] = true;
			}

			// 이미 중앙에 정렬되었다면 더 이상 움직이지 않음
			if (is_centered[i]) {
				continue;
			}

			bool change_direction = false;
			// 모든 정점이 경계에 도달했는지 확인
			for (int j = models[i].index_start; j < models[i].index_start + models[i].index_count; ++j) {
				unsigned int vertex_index = index_vec[j];
				Vertex_glm& vertex = Vertex_glm_vec[vertex_index];
				glm::vec3 next_pos = vertex.position + move_vector[i];

				// 방향에 따라 경계 확인
				if (rectspiral_direction[i] == 2 && next_pos.x > RectSpiral_boundary[i][1]) { change_direction = true; break; } // right
				if (rectspiral_direction[i] == 3 && next_pos.y > RectSpiral_boundary[i][0]) { change_direction = true; break; } // up
				if (rectspiral_direction[i] == 0 && next_pos.x < RectSpiral_boundary[i][3]) { change_direction = true; break; } // left
				if (rectspiral_direction[i] == 1 && next_pos.y < RectSpiral_boundary[i][2]) { change_direction = true; break; } // down
			}

			if (change_direction) {
				float speed = glm::length(move_vector[i]);
				rectspiral_direction[i] = (rectspiral_direction[i] + 1) % 4; // 다음 방향으로 전환

				// 방향에 따라 움직임 벡터 및 경계 수정
				switch (rectspiral_direction[i]) {
				case 3: // up
					move_vector[i] = { 0.0f, speed, 0.0f };
					RectSpiral_boundary[i][1] -= RectSpiral_shrink_rate; // 오른쪽 경계 축소
					ApplyRotation(i, 180.0f); // 위쪽 충돌
					break;
				case 0: // left
					move_vector[i] = { -speed, 0.0f, 0.0f };
					RectSpiral_boundary[i][0] -= RectSpiral_shrink_rate; // 위쪽 경계 축소
					ApplyRotation(i, 270.0f); // 왼쪽 충돌
					break;
				case 1: // down
					move_vector[i] = { 0.0f, -speed, 0.0f };
					RectSpiral_boundary[i][3] += RectSpiral_shrink_rate; // 왼쪽 경계 확장
					ApplyRotation(i, 0.0f); // 아래쪽 충돌
					break;
				case 2: // right
					move_vector[i] = { speed, 0.0f, 0.0f };
					RectSpiral_boundary[i][2] += RectSpiral_shrink_rate; // 아래쪽 경계 확장
					ApplyRotation(i, 90.0f); // 오른쪽 충돌
					break;
				}
			}

			for (int j = models[i].index_start; j < models[i].index_start + models[i].index_count; ++j) {
				unsigned int vertex_index = index_vec[j];
				Vertex_glm& vertex = Vertex_glm_vec[vertex_index];
				vertex.position += move_vector[i];
			}
		}
	}
}
void CircleSpiralMovement() {
	for (int i = 0; i < 4; ++i) {
		if (models[i].is_active) {
			if (is_shrinking[i]) {
				circle_spiral_radius[i] -= radius_change_rate;
				if (circle_spiral_radius[i] < 0.0f) {
					circle_spiral_radius[i] = 0.0f;
					is_shrinking[i] = false;
				}
			}
			else {
				circle_spiral_radius[i] += radius_change_rate;
				if (circle_spiral_radius[i] > 0.7f) {
					circle_spiral_radius[i] = 0.7f;
					is_shrinking[i] = true;
				}
			}

			glm::vec3 current_center(0.0f);
			if (models[i].index_count > 0) {
				for (int j = 0; j < models[i].index_count; ++j) {
					unsigned int vertex_index = index_vec[models[i].index_start + j];
					current_center += Vertex_glm_vec[vertex_index].position;
				}
				current_center /= models[i].index_count;
			}

			circle_spiral_angle[i] += angular_speeds[i];

			float total_angle = circle_spiral_angle[i] + circle_spiral_angle_offset[i];
			glm::vec3 next_center;
			next_center.x = circle_spiral_radius[i] * cos(total_angle);
			next_center.y = circle_spiral_radius[i] * sin(total_angle);
			next_center.z = 0.0f;

			glm::mat4 trans_to_origin = glm::translate(glm::mat4(1.0f), -current_center);
			glm::mat4 rotate_mat = glm::rotate(glm::mat4(1.0f), angular_speeds[i], glm::vec3(0.0f, 0.0f, 1.0f));
			glm::mat4 trans_to_next_pos = glm::translate(glm::mat4(1.0f), next_center);
			glm::mat4 transform_matrix = trans_to_next_pos * rotate_mat * trans_to_origin;

			for (int j = 0; j < models[i].index_count; ++j) {
				unsigned int vertex_index = index_vec[models[i].index_start + j];
				glm::vec4 position = glm::vec4(Vertex_glm_vec[vertex_index].position, 1.0f);
				Vertex_glm_vec[vertex_index].position = glm::vec3(transform_matrix * position);
			}
		}
	}
}

char* filetobuf(const char* file)
{
	FILE* fptr;
	long length;
	char* buf;
	fptr = fopen(file, "rb");						// Open file for reading
	if (!fptr)										// Return NULL on failure
		return NULL;
	fseek(fptr, 0, SEEK_END);						// Seek to the end of the file
	length = ftell(fptr);							// Find out how many bytes into the file we are
	buf = (char*)malloc(length + 1);				// Allocate a buffer for the entire length of the file and a null terminator
	fseek(fptr, 0, SEEK_SET);						// Go back to the beginning of the file
	fread(buf, length, 1, fptr);					// Read the contents of the file in to the buffer
	fclose(fptr);									// Close the file
	buf[length] = 0;								// Null terminator
	return buf;										// Return the buffer
}
void make_vertexShaders()
{
	GLchar* vertexSource;
	vertexSource = filetobuf("Vertex_Shader.glsl");
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);
	free(vertexSource);
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		std::cerr << "ERROR: vertex shader Compile Failed\n" << errorLog << std::endl;
		return;
	}
}
void make_fragmentShaders()
{
	GLchar* fragmentSource;
	fragmentSource = filetobuf("Fragment_Shader.glsl");
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);
	free(fragmentSource);
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		std::cerr << "ERROR: frag_shader Compile Failed\n" << errorLog << std::endl;
		return;
	}
}
GLuint make_shaderProgram()
{
	GLint result;
	GLchar* errorLog = NULL;
	GLuint shaderID;
	shaderID = glCreateProgram();
	glAttachShader(shaderID, vertexShader);
	glAttachShader(shaderID, fragmentShader);
	glLinkProgram(shaderID);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glGetProgramiv(shaderID, GL_LINK_STATUS, &result);
	if (!result) {
		glGetProgramInfoLog(shaderID, 512, NULL, errorLog);
		std::cerr << "ERROR: shader program Compile Failed\n" << errorLog << std::endl;
		return false;
	}
	return shaderID;
}