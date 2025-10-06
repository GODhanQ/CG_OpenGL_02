#define _CRT_SECURE_NO_WARNINGS
#include "VarANDFunc_02_Shader05.h"

auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
std::default_random_engine dre(seed);
std::uniform_real_distribution<float> urd_0_1(0.0f, 1.0f);
std::uniform_real_distribution<float> urd_m1_1(-1.0f, 1.0f);

GLuint shaderProgramID;
GLuint vertexShader;
GLuint fragmentShader;

GLuint VAO, VBO, EBO;
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

std::vector<Vertex_glm> all_vertices;
std::vector<unsigned int> all_indices;
std::vector<Shape> all_shapes;

int main(int argc, char** argv)	
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | 0x0000);
	glutInitWindowPosition(500, 10);
	glutInitWindowSize(Window_width, Window_height);
	glutCreateWindow("Example5");

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
	glutIdleFunc(drawScene);

	glutMainLoop();
}

GLvoid drawScene() {
	GLfloat rColor{ 0.3f }, gColor{ 0.3f }, bColor{ 0.3f };
	glClearColor(rColor, gColor, bColor, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(shaderProgramID);

	// drawing axis
	glBindVertexArray(axis_VAO);
	glDrawElements(GL_LINES, axis_indices.size(), GL_UNSIGNED_INT, 0);

	// drawing entities
	glBindVertexArray(VAO);

	// if during transformation, update buffer
	if (is_transforming) {
		Transform();
		UPDATE_BUFFER();
	}

	// draw according to state
	if (is_all_drawing) {
		AllDrawing();
	}
	else if (is_line2triangle) {
		Line2Triangle();
	}
	else if (is_triangle2rectangle) {
		Triangle2Rectangle();
	}
	else if (is_rectangle2heptagon) {
		Rectangle2Heptagon();
	}
	else if (is_heptagon2line) {
		Heptagon2Line();
	}

	glBindVertexArray(0);
	glutSwapBuffers();
}
GLvoid Reshape(int w, int h) {
	glViewport(0, 0, w, h);
}

void KeyBoard(unsigned char key, int x, int y) {
	switch (key) {
	case 'l':
		// state change line -> triangle, during transformation, disable other transformation
		is_line2triangle = !is_line2triangle;
		is_transforming = is_line2triangle;

		if (is_line2triangle) {
			is_triangle2rectangle = is_rectangle2heptagon = is_heptagon2line = is_all_drawing = false;
		}
		break;
	case 't':
		// state change triangle -> rectangle, during transformation, disable other transformation
		is_triangle2rectangle = !is_triangle2rectangle;
		is_transforming = is_triangle2rectangle;

		if (is_triangle2rectangle) {
			is_line2triangle = is_rectangle2heptagon = is_heptagon2line = is_all_drawing = false;
		}
		break;
	case 'r':
		// state change rectangle -> heptagon, during transformation, disable other transformation
		is_rectangle2heptagon = !is_rectangle2heptagon;
		is_transforming = is_rectangle2heptagon;

		if (is_rectangle2heptagon) {
			is_line2triangle = is_triangle2rectangle = is_heptagon2line = is_all_drawing = false;
		}
		break;
	case 'h':
		// state change heptagon -> line, during transformation, disable other transformation
		is_heptagon2line = !is_heptagon2line;
		is_transforming = is_heptagon2line;
		if (is_heptagon2line) {
			is_line2triangle = is_triangle2rectangle = is_rectangle2heptagon = is_all_drawing = false;
		}
		break;
	case 'a':
		// state change all drawing, during transformation, disable other transformation
		is_all_drawing = !is_all_drawing;
		is_transforming = is_all_drawing;

		if (is_all_drawing) {
			is_line2triangle = is_triangle2rectangle = is_rectangle2heptagon = is_heptagon2line = false;
		}
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

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// entity
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
void UPDATE_BUFFER(){
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

}

void AllDrawing() {

}

void Line2Triangle() {

}
void Triangle2Rectangle() {

}
void Rectangle2Heptagon() {

}
void Heptagon2Line() {

}