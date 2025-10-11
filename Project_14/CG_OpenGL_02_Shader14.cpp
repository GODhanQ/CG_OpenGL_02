#define _CRT_SECURE_NO_WARNINGS
#include "VarANDFunc_02_Shader14.h"

auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
std::default_random_engine dre(seed);
std::uniform_real_distribution<float> urd_0_1(0.0f, 1.0f);
std::uniform_real_distribution<float> urd_m1_1(-1.0f, 1.0f);

GLuint shaderProgramID, pointshaderProgramID;
GLuint vertexShader;
GLuint fragmentShader;

GLuint VAO, VBO, EBO;
GLuint pointVAO, pointVBO;

GLint scale_factor_loc, triangle_center_loc;
GLint rotation_axis, rotation_angle, Transform_Trigger;

std::vector<Vertex_glm> all_vertices;
std::vector<unsigned int> all_indices;

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | 0x0000);
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

	//make_vertexShaders();
	//make_fragmentShaders();
	shaderProgramID = make_shaderProgram("Vertex_Shader.glsl", "Fragment_Shader.glsl");
	pointshaderProgramID = make_shaderProgram("Point_Vertex_Shader.glsl", "Point_Fragment_Shader.glsl");
	std::cout << "Make Shader Program Completed\n";

	INIT_BUFFER();
	rotation_axis = glGetUniformLocation(shaderProgramID, "rotation_axis");
	rotation_angle = glGetUniformLocation(shaderProgramID, "rotation_angle");
	Transform_Trigger = glGetUniformLocation(shaderProgramID, "Transform_Trigger");
	scale_factor_loc = glGetUniformLocation(shaderProgramID, "scale_factor");
	std::cout << "INIT BUFFER Completed\n";

	glEnable(GL_PROGRAM_POINT_SIZE);
	
	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(KeyBoard);
	glutMouseFunc(MouseClick);
	glutIdleFunc(drawScene);

	glutMainLoop();
}

GLvoid drawScene() {
	GLfloat rColor{ 0.3f }, gColor{ 0.3f }, bColor{ 0.3f };
	glClearColor(rColor, gColor, bColor, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	Animate();

	glUseProgram(shaderProgramID);

	if (transforming) {
		sum_angle += delta_angle;
		if (sum_angle >= 360.0f) sum_angle -= 360.0f;
		else if (sum_angle <= -360.0f) sum_angle += 360.0f;
	}

	glm::vec3 uaxis = rotation_axis_value;
	float angle = sum_angle;
	glUniform3fv(rotation_axis, 1, glm::value_ptr(uaxis));
	glUniform1f(rotation_angle, angle);
	
	glBindVertexArray(VAO);

	for (int i = 0; i < 4; ++i) {
		float current_scale = 1.0f;
		if (is_scaling_animation_active && i == current_animating_triangle) {
			current_scale = 1.0f - 0.9f * sin(animation_progress * 3.14159f / 2.0f);
		}
		glUniform1f(scale_factor_loc, current_scale);

		// 삼각형의 중심 계산 및 전달
		unsigned int i1 = all_indices[i * 3 + 0];
		unsigned int i2 = all_indices[i * 3 + 1];
		unsigned int i3 = all_indices[i * 3 + 2];
		glm::vec3 center = (all_vertices[i1].position + all_vertices[i2].position + all_vertices[i3].position) / 3.0f;
		glUniform3fv(triangle_center_loc, 1, glm::value_ptr(center));

		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * i * 3));
	}

	// show axis position
	glUseProgram(pointshaderProgramID);

	Vertex_glm point_vertex = { rotation_axis_value, {1.0f, 1.0f, 1.0f} };

	glBindVertexArray(pointVAO);
	glBindBuffer(GL_ARRAY_BUFFER, pointVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex_glm), &point_vertex);

	glDrawArrays(GL_POINTS, 0, 1);
	glBindVertexArray(0);

	glutSwapBuffers();
}
GLvoid Reshape(int w, int h) {
	glViewport(0, 0, w, h);
}

void KeyBoard(unsigned char key, int x, int y) {
	switch (key) {
	case '1':
		// set triangles with first and second range
		SetupGeometry(first_range, second_range);
		UPDATE_BUFFER();

		break;
	case '2':
		// set triangels with third and fourth range
		SetupGeometry(third_range, fourth_range);
		UPDATE_BUFFER();

		break;
	case 'c':
		// rotate shape clockwise
		delta_angle = -sqrt( pow(delta_angle, 2) );
		std::cout << "clockwise: " << delta_angle << std::endl;

		break;
	case 't':
		// rotate shape counter-clockwise
		delta_angle = sqrt(pow(delta_angle, 2));
		std::cout << "counter-clockwise: " << delta_angle << std::endl;

		break;
	case 's':
		// start/stop transformation
		transforming = !transforming;
		std::cout << "transforming: " << (transforming ? " *true* " : " *false* ") << std::endl;

		break;
	case 'a':
		// set rotation axis to random axis
		glm::vec3 axis = glm::vec3(urd_m1_1(dre), urd_m1_1(dre), 1.0f);
		rotation_axis_value = glm::normalize(axis);
		sum_angle = 0.0f;
		std::cout << "Current axis: (" << axis.x << ", " << axis.y << ", " << axis.z << ")\n";

		break;
	case 'p': // 'p' 키로 스케일링 애니메이션 시작/정지
		is_scaling_animation_active = !is_scaling_animation_active;
		std::cout << "Scaling Animation: " << (is_scaling_animation_active ? " *ON* " : " *OFF* ") << std::endl;
		break;
	case 'r':
		// set rotation axis to z axis
		rotation_axis_value = glm::vec3(0.0f, 0.0f, 1.0f);
		sum_angle = 0.0f;

		break;
	case 'q':
		exit(0);
	}
}
void MouseClick(int button, int state, int x, int y) {

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
	SetupGeometry(first_range, second_range);

	std::cout << "Initializing VAO, EBO \n";
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, all_vertices.size() * sizeof(Vertex_glm), all_vertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_glm), (void*)offsetof(Vertex_glm, position));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_glm), (void*)offsetof(Vertex_glm, color));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, all_indices.size() * sizeof(unsigned int), all_indices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// for point
	glGenVertexArrays(1, &pointVAO);
	glGenBuffers(1, &pointVBO);

	glBindVertexArray(pointVAO);

	glBindBuffer(GL_ARRAY_BUFFER, pointVBO);
	
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex_glm), NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_glm), (void*)offsetof(Vertex_glm, position));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_glm), (void*)offsetof(Vertex_glm, color));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
void UPDATE_BUFFER() {
	if (all_vertices.empty() || all_indices.empty()) return;

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, all_vertices.size() * sizeof(Vertex_glm), all_vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, all_indices.size() * sizeof(unsigned int), all_indices.data(), GL_STATIC_DRAW);

	glBindVertexArray(0);
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
		std::cerr << "ERROR: vertex shader Compile Failed (" << vertPath << ")\n" << errorLog << std::endl;
		return 0;
	}

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
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

void SetupGeometry(float sq_range, float dia_range) {
	all_vertices.clear();
	all_indices.clear();

	glm::vec3 vertices_sq[4] = {
		{sq_range, sq_range, 0.0f},      // right top
		{-sq_range, sq_range, 0.0f},     // left top
		{-sq_range, -sq_range, 0.0f},    // left bottom
		{sq_range, -sq_range, 0.0f}      // right bottom
	};
	glm::vec3 vertices_dia[4] = {
		{dia_range, 0.0f, 0.0f},         // right
		{0.0f, dia_range, 0.0f},         // top
		{-dia_range, 0.0f, 0.0f},        // left
		{0.0f, -dia_range, 0.0f}         // bottom
	};

	for (auto& v : vertices_sq) {
		all_vertices.push_back({ v, {1.0f, 1.0f, 0.0f} });
	}
	for (auto& v : vertices_dia) {
		all_vertices.push_back({ v, {0.0f, 1.0f, 1.0f} });
	}

	unsigned int indices_tri[12] = {
		0, 5, 1,    // top triangle
		1, 6, 2,    // left triangle
		2, 7, 3,    // bottom triangle
		3, 4, 0     // right triangle
	};
	all_indices.insert(all_indices.end(), std::begin(indices_tri), std::end(indices_tri));
}
void Animate() {
	if (!is_scaling_animation_active) return;

	float animation_speed = 0.0002f; // 애니메이션 속도

	if (is_shrinking) {
		animation_progress += animation_speed;
		if (animation_progress >= 1.0f) {
			animation_progress = 1.0f;
			is_shrinking = false;
		}
	}
	else { // is_expanding
		animation_progress -= animation_speed;
		if (animation_progress <= 0.0f) {
			animation_progress = 0.0f;
			is_shrinking = true;
			current_animating_triangle = (current_animating_triangle + 1) % 4;
		}
	}
}