#define _CRT_SECURE_NO_WARNINGS
#include "VarANDFunc_02_Shader04.h"

auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
std::default_random_engine dre(seed);
std::uniform_real_distribution<float> urd_0_1(0.0f, 1.0f);
std::uniform_real_distribution<float> urd_m1_1(-1.0f, 1.0f);

GLuint shaderProgramID;
GLuint vertexShader;
GLuint fragmentShader;

GLuint VAO, VBO, EBO;

std::vector<Spiral> active_spirals;

// 함수 프로토타입 선언
void CreateSpiralAt(glm::vec3 center);

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | 0x0000);
	glutInitWindowPosition(500, 10);
	glutInitWindowSize(Window_width, Window_height);
	glutCreateWindow("Example4");

	glewExperimental = GL_TRUE;
	glewInit();
	std::cout << "glew initialized\n";

	// Primitive Restart 기능 활성화
	glEnable(GL_PRIMITIVE_RESTART);
	glPrimitiveRestartIndex((GLuint)-1); // 재시작 인덱스 설정

	make_vertexShaders();
	make_fragmentShaders();
	shaderProgramID = make_shaderProgram();
	std::cout << "Make Shader Program Completed\n";

	INIT_BUFFER();
	std::cout << "INIT BUFFER Completed\n";

	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(KeyBoard);
	glutMouseFunc(MouseClick);
	glutIdleFunc(drawScene);

	glutMainLoop();
}

void drawScene() {
	time_by_sec = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
	float delta_time = 1.0f / 60.0f; // 프레임 기반 시간 변화량

	// 1. 각 나선 상태 업데이트
	for (auto& spiral : active_spirals) {
		// 1.1. 새 정점 생성
		if (!spiral.generation_complete) {
			spiral.vertices.push_back({
				spiral.center,
				spiral.color,
				0.0f, // state: 확장
				0.0f, 0.0f, // expand_angle, expand_radius
				{}, {}, 0.0f, 0.0f // stop_position, shrink_center, shrink_progress, shrink_initial_radius
				});
			if (!spiral.vertices.empty() && spiral.vertices.front().state > 0.5f) {
				spiral.generation_complete = true;
			}
		}

		// 1.2. 기존 모든 정점의 상태 업데이트
		for (auto& vertex : spiral.vertices) {
			if (vertex.state < 0.5f) { // 확장 상태 (state == 0.0)
				if (vertex.expand_angle >= Max_Angle) {
					// 수축 상태로 전환
					vertex.state = 1.0f;

					// 1. 멈춘 위치 계산
					float final_radius = vertex.expand_radius;
					float final_angle_rad = glm::radians(vertex.expand_angle);
					glm::vec3 offset = { cos(final_angle_rad) * final_radius, sin(final_angle_rad) * final_radius, 0.0f };
					vertex.stop_position = vertex.initial_center + offset;

					// 2. 새로운 수축 중심 계산
					float distance_a = glm::length(vertex.stop_position - vertex.initial_center);
					vertex.shrink_center = vertex.stop_position + glm::vec3(distance_a, 0.0f, 0.0f);

					// 3. 수축 애니메이션 초기화
					vertex.shrink_initial_radius = vertex.expand_radius;
					vertex.shrink_progress = 0.0f;
				}
				else {
					// 확장
					vertex.expand_angle += rotate_speed * delta_time;
					vertex.expand_radius += Radius_change_Speed;
				}
			}
			else { // 수축 상태 (state == 1.0)
				if (vertex.shrink_progress < 1.0f) {
					float shrink_speed_multiplier = rotate_speed / (360.0f * (float)Rotation_Num);
					vertex.shrink_progress += delta_time * shrink_speed_multiplier;
				}
			}
		}

		// 1.3. 수축이 완료된 정점 제거
		auto& vertices = spiral.vertices;
		vertices.erase(
			std::remove_if(vertices.begin(), vertices.end(), [](const Vertex_glm& v) {
				return v.state > 0.5f && v.shrink_progress >= 1.0f;
				}),
			vertices.end()
		);
	}

	// 2. 모든 정점이 사라진 나선 제거
	active_spirals.erase(
		std::remove_if(active_spirals.begin(), active_spirals.end(), [](const Spiral& s) {
			return s.generation_complete && s.vertices.empty();
			}),
		active_spirals.end()
	);


	// 3. 렌더링을 위해 모든 정점 및 인덱스 취합
	std::vector<Vertex_glm> all_vertices;
	std::vector<unsigned int> all_indices;
	unsigned int base_vertex_index = 0;

	for (const auto& spiral : active_spirals) {
		if (spiral.vertices.empty()) continue;

		// 현재 나선의 정점들을 전체 목록에 추가
		all_vertices.insert(all_vertices.end(), spiral.vertices.begin(), spiral.vertices.end());

		// 현재 나선에 대한 인덱스 생성
		for (size_t i = 0; i < spiral.vertices.size(); ++i) {
			all_indices.push_back(base_vertex_index + i);
		}

		// Primitive Restart 인덱스 추가 (나선 사이에)
		all_indices.push_back((GLuint)-1);

		// 다음 나선을 위한 기본 인덱스 업데이트
		base_vertex_index += spiral.vertices.size();
	}


	// 4. 버퍼 업데이트 및 렌더링
	UPDATE_BUFFER(all_vertices, all_indices);

	// 전역 변수를 사용하여 배경색 설정
	glClearColor(bgColorR, bgColorG, bgColorB, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(shaderProgramID);
	glBindVertexArray(VAO);

	// Uniform 변수 업데이트
	glUniform1f(glGetUniformLocation(shaderProgramID, "u_rotation_num"), (float)Rotation_Num);

	glPointSize(5.0f);
	glLineWidth(2.0f); // 선 굵기 설정
	glDrawElements(drawing_type, all_indices.size(), GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);

	glutSwapBuffers();
}

GLvoid Reshape(int w, int h) {
	glViewport(0, 0, w, h);
}

void KeyBoard(unsigned char key, int x, int y) {
	int num_to_create = 0;
	switch (key) {
	case '2': num_to_create = 2; break;
	case '3': num_to_create = 3; break;
	case '4': num_to_create = 4; break;
	case '5': num_to_create = 5; break;
	case 'p':
		drawing_type = GL_POINTS;
		break;
	case 'l':
		drawing_type = GL_LINE_STRIP;
		break;
	case 'r':
		active_spirals.clear();
		UPDATE_BUFFER({}, {});
		break;
	case 'q':
		exit(0);
	}

	if (num_to_create > 0) {
		for (int i = 0; i < num_to_create; ++i) {
			// 랜덤 위치 생성 (-1.0 ~ 1.0)
			glm::vec3 random_pos = { urd_m1_1(dre), urd_m1_1(dre), 0.0f };
			CreateSpiralAt(random_pos);
		}
	}
}
void MouseClick(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		std::pair<float, float> ogl_xy = ConvertScreenToOpenGL(x, y);
		CreateSpiralAt({ ogl_xy.first, ogl_xy.second, 0.0f });
	}
}

// 나선 생성 로직을 별도 함수로 분리
void CreateSpiralAt(glm::vec3 center) {
	// 나선 개수 5개로 제한
	if (active_spirals.size() >= 5) {
		return;
	}

	// 배경색 변경
	bgColorR = urd_0_1(dre) * 0.5f; // 너무 밝지 않게 0.5 곱함
	bgColorG = urd_0_1(dre) * 0.5f;
	bgColorB = urd_0_1(dre) * 0.5f;

	Spiral new_spiral;
	new_spiral.center = center;
	new_spiral.color = { urd_0_1(dre), urd_0_1(dre), urd_0_1(dre) };
	new_spiral.generation_complete = false;

	active_spirals.push_back(std::move(new_spiral));
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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_glm), (void*)offsetof(Vertex_glm, initial_center));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_glm), (void*)offsetof(Vertex_glm, color));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex_glm), (void*)offsetof(Vertex_glm, state));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex_glm), (void*)offsetof(Vertex_glm, expand_angle));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex_glm), (void*)offsetof(Vertex_glm, expand_radius));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_glm), (void*)offsetof(Vertex_glm, stop_position));
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_glm), (void*)offsetof(Vertex_glm, shrink_center));
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(7, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex_glm), (void*)offsetof(Vertex_glm, shrink_progress));
	glEnableVertexAttribArray(7);
	glVertexAttribPointer(8, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex_glm), (void*)offsetof(Vertex_glm, shrink_initial_radius));
	glEnableVertexAttribArray(8);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
void UPDATE_BUFFER(const std::vector<Vertex_glm>& all_vertices, const std::vector<unsigned int>& all_indices)
{
	if (all_vertices.empty()) {
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
		return;
	}

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, all_vertices.size() * sizeof(Vertex_glm), all_vertices.data(), GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, all_indices.size() * sizeof(unsigned int), all_indices.data(), GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

// ... (filetobuf, make_vertexShaders, make_fragmentShaders, make_shaderProgram 함수는 동일)
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