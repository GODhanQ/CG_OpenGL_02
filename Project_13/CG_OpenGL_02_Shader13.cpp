#define _CRT_SECURE_NO_WARNINGS
#include "VarANDFunc_02_Shader13.h"

auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
std::default_random_engine dre(seed);
std::uniform_real_distribution<float> urd_0_1(0.0f, 1.0f);
std::uniform_real_distribution<float> urd_m1_1(-1.0f, 1.0f);
std::uniform_int_distribution<int> uid_0_3(0, 3);

GLuint shaderProgramID;
//GLuint outlineShaderProgramID;
GLuint vertexShader;
GLuint fragmentShader;

GLuint VAO, VBO, IBO;
//GLuint FBO;
//GLuint colorTexture, pickingTexture;
//GLuint depthRenderbuffer;



ShapeManager shape_manager;

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH); // 깊이 버퍼 추가
	glutInitWindowPosition(500, 10);
	glutInitWindowSize(Window_width, Window_height);
	glutCreateWindow("Example14");

	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		return 1;
	}
	std::cout << "glew initialized\n";

	shaderProgramID = make_shaderProgram("Vertex_Shader.glsl", "Fragment_Shader.glsl");
	if (shaderProgramID == 0) {
		std::cerr << "Shader Program creation failed. Exiting." << std::endl;
		return 1; // 셰이더 생성 실패 시 종료
	}
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

GLvoid drawScene() {
	shape_manager.PrepareShapeData();
	UPDATE_BUFFER();

	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST); // 깊이 테스트 활성화

	glUseProgram(shaderProgramID);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	// --- 상태 초기화 ---
	GLint isPickingLoc = glGetUniformLocation(shaderProgramID, "u_IsPicking");
	GLint isOutlineLoc = glGetUniformLocation(shaderProgramID, "u_IsOutline");
	glUniform1i(isPickingLoc, GL_FALSE);
	glUniform1i(isOutlineLoc, GL_FALSE);

	// 1. 모든 도형을 채워서 그리기
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	GLint base_vertex_offset = 0;
	size_t index_offset = 0;
	for (size_t i = 0; i < shape_manager.all_shapes.size(); ++i) {
		auto& s = shape_manager.all_shapes[i];
		glDrawElementsBaseVertex(s.draw_mode, s.indices.size(), GL_UNSIGNED_INT, (void*)(index_offset * sizeof(GLuint)), base_vertex_offset);
		base_vertex_offset += s.vertices.size();
		index_offset += s.indices.size();
	}

	// 2. 선택된 도형이 있다면, 그 위에 외곽선을 덧그리기
	if (selected_shape_index != -1) {
		auto& s = shape_manager.all_shapes[selected_shape_index];

		base_vertex_offset = 0;
		index_offset = 0;
		for (int i = 0; i < selected_shape_index; ++i) {
			base_vertex_offset += shape_manager.all_shapes[i].vertices.size();
			index_offset += shape_manager.all_shapes[i].indices.size();
		}

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glLineWidth(3.0f);
		glEnable(GL_POLYGON_OFFSET_LINE);
		glPolygonOffset(-1.0f, -1.0f);

		glUniform1i(isOutlineLoc, GL_TRUE); // 외곽선 모드 활성화

		glDrawElementsBaseVertex(s.draw_mode, s.indices.size(), GL_UNSIGNED_INT, (void*)(index_offset * sizeof(GLuint)), base_vertex_offset);

		// 원래 상태로 복귀
		glUniform1i(isOutlineLoc, GL_FALSE);
		glDisable(GL_POLYGON_OFFSET_LINE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glLineWidth(1.0f);
	}

	glDisable(GL_DEPTH_TEST);
	glutSwapBuffers();
}
GLvoid Reshape(int w, int h) {
	glViewport(0, 0, w, h);
}

void KeyBoard(unsigned char key, int x, int y) {
	switch (key) {
	case 'c':
		shape_manager.clear();
		selected_shape_index = -1;
		for (int i = 0; i < 10; ++i) {
			SetupGeometry();
		}
		break;
	case 'q':
		exit(0);
	}
}
void MouseClick(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		selected_shape_index = PickObject(x, y);
		if (selected_shape_index != -1) {
			std::cout << "Picked object index: " << selected_shape_index << std::endl;
		}
		else {
			std::cout << "No object picked." << std::endl;
		}
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

void INIT_BUFFER() {
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_glm), (void*)offsetof(Vertex_glm, position));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_glm), (void*)offsetof(Vertex_glm, color));
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	shape_manager.clear();
	for (int i = 0; i < 10; ++i) {
		SetupGeometry();
	}
}
void UPDATE_BUFFER() {
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, shape_manager.shape_data.all_vertices.size() * sizeof(Vertex_glm), shape_manager.shape_data.all_vertices.data(), GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, shape_manager.shape_data.all_indices.size() * sizeof(GLuint), shape_manager.shape_data.all_indices.data(), GL_DYNAMIC_DRAW);
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
GLuint make_shaderProgram(const char* vertPath, const char* fragPath) {
	GLchar* vertexSource;
	GLchar* fragmentSource;

	vertexSource = filetobuf(vertPath);
	fragmentSource = filetobuf(fragPath);

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);
	free(vertexSource);
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		std::cerr << "ERROR: vertex shader Compile Failed (" << vertPath << ")\n" << errorLog << std::endl;
		return 0;
	}

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);
	free(fragmentSource);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		std::cerr << "ERROR: frag_shader Compile Failed (" << fragPath << ")\n" << errorLog << std::endl;
		return 0;
	}

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
		std::cerr << "ERROR: shader program link Failed\n" << errorLog << std::endl;
		return 0;
	}
	return shaderID;
}

void SetupGeometry() {
	// make random shape one time
	// 0 : line, 1 : triangle, 2 : square, 3 : pentagon
	const int shape_type = uid_0_3(dre);
	glm::vec3 origin{ urd_m1_1(dre), urd_m1_1(dre), 0.0f };
	glm::vec3 color{ urd_0_1(dre), urd_0_1(dre), urd_0_1(dre) };
	const float* angles = nullptr;
	Shape shape;

	if (shape_type == 0) angles = ShapeAngles::LINE_ANGLES;
	else if (shape_type == 1) angles = ShapeAngles::TRIANGLE_ANGLES;
	else if (shape_type == 2) angles = ShapeAngles::QUAD_ANGLES;
	else if (shape_type == 3) angles = ShapeAngles::PENTAGON_ANGLES;

	shape.draw_mode = (shape_type == 0) ? GL_LINES : GL_TRIANGLE_FAN;
	shape.center = origin;

	int num_vertices = shape_type + 2;

	for (int i = 0; i < num_vertices; ++i) {
		float angle_rad = glm::radians(angles[i]);
		glm::vec3 position = {
			origin.x + shape_range * cosf(angle_rad),
			origin.y + shape_range * sinf(angle_rad),
			0.0f
		};
		shape.vertices.push_back({ position, color });
		shape.indices.push_back(i);
	}

	shape_manager.all_shapes.push_back(shape);
}
int PickObject(int x, int y) {
	// Picking Pass
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	glUseProgram(shaderProgramID);
	GLint isPickingLoc = glGetUniformLocation(shaderProgramID, "u_IsPicking");
	GLint pickingColorLoc = glGetUniformLocation(shaderProgramID, "u_PickingColor");
	glUniform1i(isPickingLoc, GL_TRUE);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // 피킹 시에는 항상 채워서 그림
	GLint base_vertex_offset = 0;
	size_t index_offset = 0;

	for (size_t i = 0; i < shape_manager.all_shapes.size(); ++i) {
		auto& s = shape_manager.all_shapes[i];
		int id = (int)i + 1;
		int r = (id & 0x000000FF) >> 0;
		int g = (id & 0x0000FF00) >> 8;
		int b = (id & 0x00FF0000) >> 16;
		glUniform3f(pickingColorLoc, r / 255.0f, g / 255.0f, b / 255.0f);

		glDrawElementsBaseVertex(s.draw_mode, s.indices.size(), GL_UNSIGNED_INT, (void*)(index_offset * sizeof(GLuint)), base_vertex_offset);
		base_vertex_offset += s.vertices.size();
		index_offset += s.indices.size();
	}

	glFlush();
	glFinish();

	unsigned char pixel[4];
	int inverted_y = glutGet(GLUT_WINDOW_HEIGHT) - y;
	glReadPixels(x, inverted_y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixel);

	int pickedID = pixel[0] + (pixel[1] << 8) + (pixel[2] << 16);

	glUniform1i(isPickingLoc, GL_FALSE);
	glDisable(GL_DEPTH_TEST);

	if (pickedID == 0 || pickedID > shape_manager.all_shapes.size()) {
		return -1;
	}
	return pickedID - 1;
}