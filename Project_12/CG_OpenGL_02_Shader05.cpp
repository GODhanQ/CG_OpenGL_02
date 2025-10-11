#define _CRT_SECURE_NO_WARNINGS
#include "VarANDFunc_02_Shader05.h"

auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
std::default_random_engine dre(seed);
std::uniform_real_distribution<float> urd_0_1(0.0f, 1.0f);
std::uniform_real_distribution<float> urd_m1_1(-1.0f, 1.0f);

GLuint shaderProgramID;
GLuint vertexShader;
GLuint fragmentShader;

GLuint VAO, VBO, EBO, IBO_Lines, IBO_Fill;
GLuint axis_VAO, axis_VBO, axis_EBO;

// axis lines
std::vector<Vertex_glm> axis_vertices = {
	// x axis
	{ {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f} },
	{ { 1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f} },
	// y axis
	{ {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f, 0.0f} },
	{ {0.0f,  1.0f, 0.0f}, {0.0f, 1.0f, 0.0f} },
};
std::vector<unsigned int> axis_indices = {
	0, 1,
	2, 3
};

//std::vector<Vertex_glm> all_vertices;
//std::vector<unsigned int> all_indices;
//std::vector<Shape> all_shapes;

ShapeManager shape_manager;

GLint angles_loc;

int main(int argc, char** argv)	
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | 0x0000);
	glutInitWindowPosition(500, 10);
	glutInitWindowSize(Window_width, Window_height);
	glutCreateWindow("Example12");

	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		return 1;
	}
	std::cout << "glew initialized\n";

	make_vertexShaders();
	make_fragmentShaders();
	shaderProgramID = make_shaderProgram();
	std::cout << "Make Shader Program Completed\n";

	//angles_loc = glGetUniformLocation(shaderProgramID, "angles");

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
	GLfloat rColor{ 0.3f }, gColor{ 0.3f }, bColor{ 0.3f };
	glClearColor(rColor, gColor, bColor, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	Transform();
	UPDATE_BUFFER();

	glUseProgram(shaderProgramID);

	// drawing axis : only all_drawing mode
	if (is_all_drawing) {
		glBindVertexArray(axis_VAO);

		glLineWidth(2.0f);
		glDrawElements(GL_LINES, axis_indices.size(), GL_UNSIGNED_INT, 0);
	}

	auto& all_shapes = shape_manager.all_shapes;

	glBindVertexArray(VAO);

	for (size_t shape_index = 0; shape_index < all_shapes.size(); ++shape_index) {
		auto& s = all_shapes[shape_index];
		GLint base_vertex_offset = (GLint)(shape_index * 7);

		if (s.current_state == 0) {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO_Lines);
			glLineWidth(5.0f);
			glDrawElementsBaseVertex(GL_LINES, 2, GL_UNSIGNED_INT, 0, base_vertex_offset);
		}
		else {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO_Fill);
			glDrawElementsBaseVertex(GL_TRIANGLE_FAN, 7, GL_UNSIGNED_INT, 0, base_vertex_offset);
		}
	}

	glBindVertexArray(0);
	glutSwapBuffers();
}
GLvoid Reshape(int w, int h) {
	glViewport(0, 0, w, h);
}

void KeyBoard(unsigned char key, int x, int y) {
	shape_manager.all_shapes.clear(); // reset all shapes

	switch (key) {
	case 'l':
		// state change line -> triangle, during transformation, disable other transformation
		is_all_drawing = false;
		do_second_process = false;
		current_shape_state = 0;

		shape_manager.all_shapes.push_back(Shape(glm::vec3(0.0f), current_shape_state));

		break;
	case 't':
		// state change triangle -> rectangle, during transformation, disable other transformation
		is_all_drawing = false;
		do_second_process = false;
		current_shape_state = 1;

		shape_manager.all_shapes.push_back(Shape(glm::vec3(0.0f), current_shape_state));

		break;
	case 'r':
		// state change rectangle -> heptagon, during transformation, disable other transformation
		is_all_drawing = false;
		do_second_process = false;
		current_shape_state = 2;

		shape_manager.all_shapes.push_back(Shape(glm::vec3(0.0f), current_shape_state));

		break;
	case 'h':
		// state change heptagon -> line, during transformation, disable other transformation
		is_all_drawing = false;
		do_second_process = false;
		current_shape_state = 3;

		shape_manager.all_shapes.push_back(Shape(glm::vec3(0.0f), current_shape_state));

		break;
	case 'a':
		// state change all drawing, during transformation, disable other transformation
		is_all_drawing = true;
		do_second_process = false;
		current_shape_state = -1;

		shape_manager.all_shapes.push_back(Shape(glm::vec3(0.5f, 0.5f, 0.0f), 1));
		shape_manager.all_shapes.push_back(Shape(glm::vec3(-0.5f, 0.5f, 0.0f), 0));
		shape_manager.all_shapes.push_back(Shape(glm::vec3(-0.5f, -0.5f, 0.0f), 2));
		shape_manager.all_shapes.push_back(Shape(glm::vec3(0.5f, -0.5f, 0.0f), 3));

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
	std::cout << "Initializing VAO, EBO \n";

	// axis
	glGenVertexArrays(1, &axis_VAO);
	glGenBuffers(1, &axis_VBO);
	glGenBuffers(1, &axis_EBO);

	glBindVertexArray(axis_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, axis_VBO);
	glBufferData(GL_ARRAY_BUFFER, axis_vertices.size() * sizeof(Vertex_glm), axis_vertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_glm), (void*)offsetof(Vertex_glm, position));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_glm), (void*)offsetof(Vertex_glm, color));
	glEnableVertexAttribArray(1);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, axis_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, axis_indices.size() * sizeof(unsigned int), axis_indices.data(), GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// entity
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(shape_manager.shape_data.positions.size() * sizeof(glm::vec3)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// - IBO for lines (line segments)
	glGenBuffers(1, &IBO_Lines);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO_Lines);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(shared_ibo_line_id), shared_ibo_line_id, GL_DYNAMIC_DRAW);

	// - IBO for fill (triangles, rectangles, heptagons)
	glGenBuffers(1, &IBO_Fill);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO_Fill);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(shared_ibo_fan_id), shared_ibo_fan_id, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
void UPDATE_BUFFER() {
	if (shape_manager.shape_data.empty()) return;

	auto& sd = shape_manager.shape_data;
	size_t positions_size = sd.positions.size() * sizeof(glm::vec3);
	size_t colors_size = sd.colors.size() * sizeof(glm::vec3);
	size_t total_size = positions_size + colors_size;

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, total_size, nullptr, GL_DYNAMIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, positions_size, sd.positions.data());
	glBufferSubData(GL_ARRAY_BUFFER, positions_size, colors_size, sd.colors.data());

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)positions_size);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
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

void Transform() {
	for (auto& s : shape_manager.all_shapes) {
		// 1. Angles 가져오기
		const float* Angles = nullptr;
		float radius = (is_all_drawing) ? radius_for_all : radius_for_one;

		if (s.current_state == 0) Angles = ShapeAngles::LINE_ANGLES;
		else if (s.current_state == 1) Angles = ShapeAngles::TRIANGLE_ANGLES;
		else if (s.current_state == 2) Angles = ShapeAngles::QUAD_ANGLES;
		else Angles = ShapeAngles::PENTAGON_ANGLES;

		// 2. 변환 상태 전환
		if (is_all_drawing) {
			if (s.is_in_progress == false) {
				s.current_state = (s.current_state + 1) % 4;
				s.is_in_progress = true;
				std::cout << "go next state: " << s.current_state << std::endl;
				continue;
			}
		}
		else {
			if (s.is_in_progress == false) {
				if (do_second_process) continue;
				s.current_state = (s.current_state + 1) % 4;
				s.is_in_progress = true;
				do_second_process = true;
				continue;
			}
		}

		// 3. 정점 이동
		bool all_vertices_arrived = true;
		for (int i = 0; i < 6; ++i) {
			float target_x_origin = radius * cos(glm::radians(Angles[i]));
			float target_y_origin = radius * sin(glm::radians(Angles[i]));

			glm::vec2 target_pos = glm::vec2(
				s.center.x + target_x_origin,
				s.center.y + target_y_origin
			);

			glm::vec2 current_pos = glm::vec2(s.vertices[i].position.x, s.vertices[i].position.y);
			glm::vec2 direction_vector = target_pos - current_pos;
			float length = glm::length(direction_vector);
			float speed = (!is_all_drawing) ? moving_speed_for_one : moving_speed_for_all;

			if (length > 0.01f) {
				all_vertices_arrived = false;
				glm::vec2 normalized_dir = glm::normalize(direction_vector);
				glm::vec2 movement = normalized_dir * speed;

				s.vertices[i].position.x += movement.x;
				s.vertices[i].position.y += movement.y;
			}
			else {
				s.vertices[i].position.x = target_pos.x;
				s.vertices[i].position.y = target_pos.y;
			}
		}

		// 4. 최종 플래그 업데이트
		if (all_vertices_arrived) {
			s.is_in_progress = false;
		}
	}

	shape_manager.PrepareShapeData();
}