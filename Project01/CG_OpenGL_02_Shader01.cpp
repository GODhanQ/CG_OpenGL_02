#define _CRT_SECURE_NO_WARNINGS
#include "VarANDFunc_02_Shader01.h"

constexpr int COLOR_DEFAULT{ 0x7 };
constexpr int COLOR_RED{ 0xC };
constexpr int COLOR_YELLOW{ 0xE };
auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
std::default_random_engine dre(seed);
std::uniform_real_distribution<float> urd_0_1(0.0f, 1.0f);
std::uniform_real_distribution<float> urd_m1_1(-1.0f, 1.0f);

GLuint shaderProgramID;
GLuint vertexShader;
GLuint fragmentShader;

GLuint VAO, VBO_pos, VBO_color, VBO, EBO;

DrawBatchManager drawBatchManager;

// Vertex Data : position(x,y,z), color(r,g,b)
std::vector<Vertex_glm> Vertex_glm_vec;
// Vertex Index (Triangle)
std::vector<unsigned int> index_vec;
// Shape models : Shape
std::vector<Shape> Model_vec;

void main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | 0x0000);
	glutInitWindowPosition(100, glutGet(GLUT_WINDOW_HEIGHT) - Window_height);
	glutInitWindowSize(Window_width, Window_height);
	glutCreateWindow("Example1");

	glewExperimental = GL_TRUE;
	glewInit();

	make_vertexShaders();
	make_fragmentShaders();
	shaderProgramID = make_shaderProgram();

	INIT_BUFFER();

	glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);

	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(SpecialKeyboard);
	glutSpecialUpFunc(SpecialKeyboardUp);
	glutMouseFunc(MouseClick);

	glutMainLoop();
}

GLvoid drawScene() {
	GLfloat rColor{ 0.3f }, gColor{ 0.3f }, bColor{ 0.3f };
	glClearColor(rColor, gColor, bColor, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(shaderProgramID);

	// Î™®Îì† Í∑∏Î¶¨Í∏∞Î? ?úÏûë?òÍ∏∞ ?ÑÏóê Picking Î∞?Override Î™®ÎìúÎ•?ÎπÑÌôú?±Ìôî
	glUniform1i(glGetUniformLocation(shaderProgramID, "u_IsPicking"), GL_FALSE);
	glUniform1i(glGetUniformLocation(shaderProgramID, "u_UseOverrideColor"), GL_FALSE);

	if (moving_flag) {
		Translation();
	}

	if (!index_vec.empty()) {
		glBindVertexArray(VAO);

		glPointSize(10.0);
		glLineWidth(5.0);

		drawBatchManager.prepareDrawCalls(Model_vec);
		drawBatchManager.drawAll();
	}

	// ?†ÌÉù??Í∞ùÏ≤¥Í∞Ä ?àÎã§Î©? Í∑??ÑÏóê ?òÏù¥?ºÏù¥???∏Í≥Ω??Î•??ßÍ∑∏Î¶?
	if (selected_model_index != -1 && selected_model_index < Model_vec.size()) {
		Shape& selected_shape = Model_vec[selected_model_index];

		// ?òÏù¥?ºÏù¥??Î™®Îìú ?úÏÑ±??
		GLint useOverrideLoc = glGetUniformLocation(shaderProgramID, "u_UseOverrideColor");
		GLint overrideColorLoc = glGetUniformLocation(shaderProgramID, "u_OverrideColor");
		glUniform1i(useOverrideLoc, GL_TRUE);
		glUniform3f(overrideColorLoc, 1.0f, 1.0f, 0.0f); // ?∏Î??âÏúºÎ°??§Ï†ï

		// ?∏Í≥Ω?†ÏùÑ Í∑∏Î¶¨Í∏??ÑÌï¥ ?¥Î¶¨Í≥?Î™®Îìú Î≥ÄÍ≤?
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glLineWidth(3.0); // ?∏Í≥Ω???êÍªò

		glBindVertexArray(VAO);
		glDrawElementsBaseVertex(
			selected_shape.draw_mode,
			selected_shape.index_count,
			GL_UNSIGNED_INT,
			(void*)selected_shape.index_offset,
			selected_shape.base_vertex
		);


		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glUniform1i(useOverrideLoc, GL_FALSE);
	}

	glutSwapBuffers();
}

GLvoid Reshape(int w, int h) {
	glViewport(0, 0, w, h);
}

void Keyboard(unsigned char key, int x, int y) {
	if (key != 'l') {
		active_line_strip_model_index = -1;
	}

	switch (key) {
	case 'p':
		if (DrawLine_mode) {
			std::cout << "Please deactivate Line Drawing Mode ('l') first.\n";
			break;
		}
		if (DrawTriangle_mode || DrawSquare_mode) {
			std::cout << "Another drawing mode is already selected. Please deselect it first.\n";
			break;
		}
		DrawPoint_mode = !DrawPoint_mode;

		std::cout << "Point Drawing Mode :";
		ChangeConsoleColor(DrawPoint_mode ? COLOR_YELLOW : COLOR_DEFAULT);
		std::cout << (DrawPoint_mode ? " On" : " Off") << '\n';
		ChangeConsoleColor(COLOR_DEFAULT);
		break;
	case 'l':
		if (DrawPoint_mode || DrawTriangle_mode || DrawSquare_mode) {
			std::cout << "Another drawing mode is already selected. Please deselect it first.\n";
			break;
		}
		DrawLine_mode = !DrawLine_mode;

		if (!DrawLine_mode) {
			active_line_strip_model_index = -1;
		}

		std::cout << "Line Drawing Mode :";
		ChangeConsoleColor(DrawLine_mode ? COLOR_YELLOW : COLOR_DEFAULT);
		std::cout << (DrawLine_mode ? " On" : " Off") << '\n';
		ChangeConsoleColor(COLOR_DEFAULT);
		break;
	case 't':
		if (DrawLine_mode) {
			std::cout << "Please deactivate Line Drawing Mode ('l') first.\n";
			break;
		}
		if (DrawPoint_mode || DrawSquare_mode) {
			std::cout << "Another drawing mode is already selected. Please deselect it first.\n";
			break;
		}
		DrawTriangle_mode = !DrawTriangle_mode;

		std::cout << "Triangle Drawing Mode :";
		ChangeConsoleColor(DrawTriangle_mode ? COLOR_YELLOW : COLOR_DEFAULT);
		std::cout << (DrawTriangle_mode ? " On" : " Off") << '\n';
		ChangeConsoleColor(COLOR_DEFAULT);
		break;
	case 'r':
		if (DrawLine_mode) { // ?ºÏù∏ Î™®ÎìúÍ∞Ä ÏºúÏ†∏ ?àÏúºÎ©??§Î•∏ Î™®Îìú ?†ÌÉù Î∂àÍ?
			std::cout << "Please deactivate Line Drawing Mode ('l') first.\n";
			break;
		}
		if (DrawPoint_mode || DrawTriangle_mode) {
			std::cout << "Another drawing mode is already selected. Please deselect it first.\n";
			break;
		}
		DrawSquare_mode = !DrawSquare_mode;

		std::cout << "Square Drawing Mode :";
		ChangeConsoleColor(DrawSquare_mode ? COLOR_YELLOW : COLOR_DEFAULT);
		std::cout << (DrawSquare_mode ? " On" : " Off") << '\n';
		ChangeConsoleColor(COLOR_DEFAULT);
		break;
	case 's': // Í∞ùÏ≤¥ ?†ÌÉù Î™®Îìú ?†Í? ??
		is_picking_mode = !is_picking_mode;
		if (is_picking_mode) {
			std::cout << "Picking Mode:";
			ChangeConsoleColor(COLOR_YELLOW);
			std::cout << " ON    ";
			ChangeConsoleColor(COLOR_DEFAULT);
			std::cout << "Click to select an object.\n";

			// ?§Î•∏ Î™®Îì† Í∑∏Î¶¨Í∏?Î™®Îìú ÎπÑÌôú?±Ìôî
			DrawPoint_mode = DrawLine_mode = DrawTriangle_mode = DrawSquare_mode = false;
			active_line_strip_model_index = -1;
			selected_model_index = -1; // ?†ÌÉù Î™®Îìú ÏßÑÏûÖ ??Í∏∞Ï°¥ ?†ÌÉù ?¥Ï†ú
		}
		else {
			std::cout << "Picking Mode:";
			ChangeConsoleColor(COLOR_YELLOW);
			std::cout << " OFF.\n";
			ChangeConsoleColor(COLOR_DEFAULT);
			selected_model_index = -1; // ?†ÌÉù ?¥Ï†ú
		}
		glutPostRedisplay(); // ?îÎ©¥ Í∞±Ïã†?òÏó¨ ?òÏù¥?ºÏù¥???úÍ±∞
		break;
	case 'c':
		Vertex_glm_vec.clear();
		index_vec.clear();
		Model_vec.clear();

		Current_Diagram_Count = 0;
		active_line_strip_model_index = -1;
		selected_model_index = -1;
		std::cout << "Cleared all diagrams.\n";

		UPDATE_BUFFER();
		glutPostRedisplay();
		break;
	case 'q':
		exit(0);
	}
}

void SpecialKeyboard(int key, int x, int y) {
	if (key < 0 || key >= 256) {
		std::cout << "Invalid special key code: " << key << std::endl;
		return;
	}

	switch (key) {
	case GLUT_KEY_UP:
		movement_vec.y += movement_speed;

		std::cout << "Pressed UP Key\n";
		std::cout << "movement_vec.y: " << movement_vec.y << "\n";
		break;
	case GLUT_KEY_DOWN:
		movement_vec.y -= movement_speed;

		std::cout << "Pressed DOWN Key\n";
		std::cout << "movement_vec.y: " << movement_vec.y << "\n";
		break;
	case GLUT_KEY_LEFT:
		movement_vec.x -= movement_speed;

		std::cout << "Pressed LEFT Key\n";
		std::cout << "movement_vec.x: " << movement_vec.x << "\n";
		break;
	case GLUT_KEY_RIGHT:
		movement_vec.x += movement_speed;

		std::cout << "Pressed RIGHT Key\n";
		std::cout << "movement_vec.x: " << movement_vec.x << "\n";
		break;
	}

	moving_flag = (movement_vec.x != 0.0f || movement_vec.y != 0.0f);

	glutPostRedisplay();
}

void SpecialKeyboardUp(int key, int x, int y) {
	if (key < 0 || key >= 256) {
		return;
	}

	switch (key) {
	case GLUT_KEY_UP:
		movement_vec.y -= movement_speed;

		std::cout << "Unpressed UP Key\n";
		std::cout << "stop moving. movement_vec.y: " << movement_vec.y << "\n";
		break;
	case GLUT_KEY_DOWN:
		movement_vec.y += movement_speed;

		std::cout << "Unpressed DOWN Key\n";
		std::cout << "stop moving. movement_vec.y: " << movement_vec.y << "\n";
		break;
	case GLUT_KEY_LEFT:
		movement_vec.x += movement_speed;

		std::cout << "Unpressed LEFT Key\n";
		std::cout << "stop moving. movement_vec.x: " << movement_vec.x << "\n";
		break;
	case GLUT_KEY_RIGHT:
		movement_vec.x -= movement_speed;

		std::cout << "Unpressed RIGHT Key\n";
		std::cout << "stop moving. movement_vec.x: " << movement_vec.x << "\n";
		break;
	}

	moving_flag = (movement_vec.x != 0.0f || movement_vec.y != 0.0f);

	glutPostRedisplay();
}

void MouseClick(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		if (is_picking_mode) {
			selected_model_index = PickObject(x, y);
			if (selected_model_index != -1) {
				std::cout << "Object " << selected_model_index << " selected.\n";
			}
			else {
				std::cout << "No object selected.  ";
				std::cout << "Picking Mode:";
				ChangeConsoleColor(COLOR_YELLOW);
				std::cout << ((is_picking_mode) ? " ON." : " OFF.") << '\n';
				ChangeConsoleColor(COLOR_DEFAULT);
			}
			glutPostRedisplay(); // ?†ÌÉù Í≤∞Í≥ºÎ•??îÎ©¥??Î∞òÏòÅ
			return;
		}

		if (Current_Diagram_Count >= 10 && active_line_strip_model_index == -1) { // ???ÑÌòï??ÎßåÎì§ ?åÎßå Í∞úÏàò Ï≤¥ÌÅ¨
			std::cout << "Maximum number of Diagram reached (10). Cannot add more.\n";
			return;
		}
		std::pair<float, float> ogl_xy = ConvertMouseWxy2GLxy(x, y);
		float ogl_x = ogl_xy.first;
		float ogl_y = ogl_xy.second;
		std::cout << "Mouse Clicked at ( " << ogl_x << ", " << ogl_y << " )\n";

		if (DrawPoint_mode) {
			active_line_strip_model_index = -1; // ?§Î•∏ Î™®ÎìúÎ•??¥Î¶≠?òÎ©¥ ?ºÏù∏ Í∑∏Î¶¨Í∏?Ï¢ÖÎ£å
			CreatePointAtOrigin(ogl_x, ogl_y);
		}
		else if (DrawLine_mode) {
			AddVertexToLineStrip(ogl_x, ogl_y);
		}
		else if (DrawTriangle_mode) {
			active_line_strip_model_index = -1; // ?§Î•∏ Î™®ÎìúÎ•??¥Î¶≠?òÎ©¥ ?ºÏù∏ Í∑∏Î¶¨Í∏?Ï¢ÖÎ£å
			CreateTriangleAtOrigin(ogl_x, ogl_y);
		}
		else if (DrawSquare_mode) {
			active_line_strip_model_index = -1; // ?§Î•∏ Î™®ÎìúÎ•??¥Î¶≠?òÎ©¥ ?ºÏù∏ Í∑∏Î¶¨Í∏?Ï¢ÖÎ£å
			CreateSquareAtOrigin(ogl_x, ogl_y);
		}
		else {
			std::cout << "No drawing mode selected. Please select a mode.\n";
			return;
		}

		UPDATE_BUFFER();
		glutPostRedisplay();
	}
}

std::pair<float, float> ConvertMouseWxy2GLxy(int x, int y) {
	int width = glutGet(GLUT_WINDOW_WIDTH);
	int height = glutGet(GLUT_WINDOW_HEIGHT);

	float ogl_x = (2.0f * x) / width - 1.0f;
	float ogl_y = 1.0f - (2.0f * y) / height;

	return { ogl_x, ogl_y };
}

// uniform Î≥Ä?òÎäî Î¨¥Ï°∞Í±??†ÎãπÎ∞õÍ∏∞
// ?ïÏ†ê ?ÑÏπò Î∞îÎÄåÎ©¥ Î≤ÑÌçº???§Ïãú Î≥¥ÎÇ¥Ï£ºÍ∏∞

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
	//--- Î≤ÑÌÖç???∏Ïù¥???ΩÏñ¥ ?Ä?•ÌïòÍ≥?Ïª¥Ìåå???òÍ∏∞
	//--- filetobuf: ?¨Ïö©?êÏ†ï???®ÏàòÎ°??çÏä§?∏Î? ?ΩÏñ¥??Î¨∏Ïûê?¥Ïóê ?Ä?•Ìïò???®Ïàò
	vertexSource = filetobuf("Vertex.glsl");
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		std::cerr << "ERROR: vertex shader Ïª¥Ìåå???§Ìå®\n" << errorLog << std::endl;
		return;
	}
}
void make_fragmentShaders()
{
	GLchar* fragmentSource;
	//--- ?ÑÎûòÍ∑∏Î®º???∏Ïù¥???ΩÏñ¥ ?Ä?•ÌïòÍ≥?Ïª¥Ìåå?ºÌïòÍ∏?
	fragmentSource = filetobuf("Fragment.glsl");	// ?ÑÎûòÍ∑∏ÏÑ∏?¥Îçî ?ΩÏñ¥?§Í∏∞
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		std::cerr << "ERROR: frag_shader Ïª¥Ìåå???§Ìå®\n" << errorLog << std::endl;
		return;
	}
}
GLuint make_shaderProgram()
{
	GLint result;
	GLchar* errorLog = NULL;
	GLuint shaderID;
	shaderID = glCreateProgram();					//--- ?∏Ïù¥???ÑÎ°úÍ∑∏Îû® ÎßåÎì§Í∏?
	glAttachShader(shaderID, vertexShader);			//--- ?∏Ïù¥???ÑÎ°úÍ∑∏Îû®??Î≤ÑÌÖç???∏Ïù¥??Î∂ôÏù¥Í∏?
	glAttachShader(shaderID, fragmentShader);		//--- ?∏Ïù¥???ÑÎ°úÍ∑∏Îû®???ÑÎûòÍ∑∏Î®º???∏Ïù¥??Î∂ôÏù¥Í∏?
	glLinkProgram(shaderID);						//--- ?∏Ïù¥???ÑÎ°úÍ∑∏Îû® ÎßÅÌÅ¨?òÍ∏∞
	glDeleteShader(vertexShader);					//--- ?∏Ïù¥??Í∞ùÏ≤¥Î•??∏Ïù¥???ÑÎ°úÍ∑∏Îû®??ÎßÅÌÅ¨?àÏùå?ºÎ°ú, ?∏Ïù¥??Í∞ùÏ≤¥ ?êÏ≤¥????†ú Í∞Ä??
	glDeleteShader(fragmentShader);
	glGetProgramiv(shaderID, GL_LINK_STATUS, &result); // ---?∏Ïù¥?îÍ? ???∞Í≤∞?òÏóà?îÏ? Ï≤¥ÌÅ¨?òÍ∏∞
	if (!result) {
		glGetProgramInfoLog(shaderID, 512, NULL, errorLog);
		std::cerr << "ERROR: shader program ?∞Í≤∞ ?§Ìå®\n" << errorLog << std::endl;
		return false;
	}
	//glUseProgram(shaderID);							//--- ÎßåÎì§?¥ÏßÑ ?∏Ïù¥???ÑÎ°úÍ∑∏Îû® ?¨Ïö©?òÍ∏∞
	//--- ?¨Îü¨ Í∞úÏùò ?∏Ïù¥?îÌîÑÎ°úÍ∑∏??ÎßåÎì§ ???àÍ≥†, Í∑?Ï§??úÍ∞ú???ÑÎ°úÍ∑∏Îû®???¨Ïö©?òÎ†§Î©?
	//--- glUseProgram ?®ÏàòÎ•??∏Ï∂ú?òÏó¨ ?¨Ïö© ???πÏ†ï ?ÑÎ°úÍ∑∏Îû®??ÏßÄ?ïÌïú??
	//--- ?¨Ïö©?òÍ∏∞ ÏßÅÏ†Ñ???∏Ï∂ú?????àÎã§.
	return shaderID;
}

void INIT_BUFFER()
{
	std::cout << "Initializing VAO, EBO \n";

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_glm), (void*)offsetof(Vertex_glm, position));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_glm), (void*)offsetof(Vertex_glm, color));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}
void UPDATE_BUFFER()
{
	if (Vertex_glm_vec.empty() || index_vec.empty()) return;
	std::cout << "Updating VBO, EBO \n";

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, Vertex_glm_vec.size() * sizeof(Vertex_glm), Vertex_glm_vec.data(), GL_DYNAMIC_DRAW);
	std::cout << "Rebinding VBO with size: " << Vertex_glm_vec.size() * sizeof(Vertex_glm) << " Count of Vertex : " << Vertex_glm_vec.size() << "\n";

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_vec.size() * sizeof(unsigned int), index_vec.data(), GL_DYNAMIC_DRAW);
	std::cout << "Rebinding EBO with size: " << index_vec.size() * sizeof(unsigned int) << " Count of Index : " << index_vec.size() << "\n";

	// Î≤ÑÌçº Î∞îÏù∏???¥Ï†ú
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	std::cout << "Buffer Update Completed\n";
}

void CreatePointAtOrigin(float ogl_x, float ogl_y) {
	// now create a new origin and make a point from origin with random color
	Vertex_glm v1;
	Shape shape;
	float x1 = ogl_x, y1 = ogl_y, z1 = 0.0f;
	v1.position = glm::vec3(x1, y1, z1);

	float r1 = urd_0_1(dre), g1 = urd_0_1(dre), b1 = urd_0_1(dre);
	v1.color = glm::vec3(r1, g1, b1);

	// Count : 1, Index: (0) // Count : 2, Index: (1) // Count : 3, Index: (2) ...
	int base_vertex = Vertex_glm_vec.size();
	size_t index_offset = index_vec.size();
	Vertex_glm_vec.push_back(v1);

	shape.draw_mode = GL_POINTS; shape.index_count = 1; shape.base_vertex = base_vertex;
	shape.index_offset = index_offset * sizeof(unsigned int);

	Current_Diagram_Count++;
	index_vec.push_back(0);
	Model_vec.push_back(shape);

	std::cout << "Created Point at (" << x1 << ", " << y1 << ")\n";
}
void AddVertexToLineStrip(float ogl_x, float ogl_y) {
	Vertex_glm v;
	v.position = glm::vec3(ogl_x, ogl_y, 0.0f);
	v.color = glm::vec3(urd_0_1(dre), urd_0_1(dre), urd_0_1(dre));

	if (active_line_strip_model_index == -1) {
		Shape shape;
		shape.draw_mode = GL_LINE_STRIP;
		shape.index_count = 1; // ?úÏûë?Ä ?ïÏ†ê 1Í∞?
		shape.base_vertex = Vertex_glm_vec.size();
		shape.index_offset = index_vec.size() * sizeof(unsigned int);

		Vertex_glm_vec.push_back(v);
		index_vec.push_back(0); // Î°úÏª¨ ?∏Îç±??0

		Model_vec.push_back(shape);
		active_line_strip_model_index = Model_vec.size() - 1; // ?àÎ°ú ÎßåÎì† Shape???∏Îç±???Ä??
		Current_Diagram_Count++;
		std::cout << "Started a new Line Strip.\n";
	}
	else {
		Shape& shape = Model_vec[active_line_strip_model_index];

		Vertex_glm_vec.push_back(v);
		index_vec.push_back(shape.index_count);

		shape.index_count++;

		std::cout << "Added a vertex to the current Line Strip. Total vertices: " << shape.index_count << "\n";
	}
}
void CreateTriangleAtOrigin(float ogl_x, float ogl_y) {
	// now create a new origin and make a triangle from origin with random color
	Vertex_glm v1, v2, v3;
	Shape shape;

	float x1 = ogl_x, y1 = ogl_y + Triangle_range, z1 = 0.0f;
	float x2 = ogl_x - Triangle_range, y2 = ogl_y - Triangle_range, z2 = 0.0f;
	float x3 = ogl_x + Triangle_range, y3 = ogl_y - Triangle_range, z3 = 0.0f;
	v1.position = glm::vec3(x1, y1, z1);
	v2.position = glm::vec3(x2, y2, z2);
	v3.position = glm::vec3(x3, y3, z3);

	float r1 = urd_0_1(dre), g1 = urd_0_1(dre), b1 = urd_0_1(dre);
	float r2 = urd_0_1(dre), g2 = urd_0_1(dre), b2 = urd_0_1(dre);
	float r3 = urd_0_1(dre), g3 = urd_0_1(dre), b3 = urd_0_1(dre);
	v1.color = glm::vec3(r1, g1, b1);
	v2.color = glm::vec3(r2, g2, b2);
	v3.color = glm::vec3(r3, g3, b3);

	// Count : 1, Index: (0, 1, 2) // Count : 2, Index: (3, 4, 5) // Count : 3, Index: (6, 7, 8) ...
	int base_vertex = Vertex_glm_vec.size();
	size_t index_offset = index_vec.size();
	Vertex_glm_vec.push_back(v1); Vertex_glm_vec.push_back(v2); Vertex_glm_vec.push_back(v3);

	shape.draw_mode = GL_TRIANGLES; shape.index_count = 3; shape.base_vertex = base_vertex;
	shape.index_offset = index_offset * sizeof(unsigned int);

	Current_Diagram_Count++;
	index_vec.push_back(0);
	index_vec.push_back(1);
	index_vec.push_back(2);
	Model_vec.push_back(shape);

	std::cout << "Created Triangle at (" << x1 << ", " << y1 << "), (" << x2 << ", " << y2 << "), (" << x3 << ", " << y3 << ")\n";
}
void CreateSquareAtOrigin(float ogl_x, float ogl_y) {
	// now create a new origin and make a square from origin with random color
	Vertex_glm v1, v2, v3, v4;
	Shape shape;

	float x1 = ogl_x + Triangle_range, y1 = ogl_y + Triangle_range, z1 = 0.0f;
	float x2 = ogl_x + Triangle_range, y2 = ogl_y - Triangle_range, z2 = 0.0f;
	float x3 = ogl_x - Triangle_range, y3 = ogl_y - Triangle_range, z3 = 0.0f;
	float x4 = ogl_x - Triangle_range, y4 = ogl_y + Triangle_range, z4 = 0.0f;
	v1.position = glm::vec3(x1, y1, z1);
	v2.position = glm::vec3(x2, y2, z2);
	v3.position = glm::vec3(x3, y3, z3);
	v4.position = glm::vec3(x4, y4, z4);

	float r1 = urd_0_1(dre), g1 = urd_0_1(dre), b1 = urd_0_1(dre);
	float r2 = urd_0_1(dre), g2 = urd_0_1(dre), b2 = urd_0_1(dre);
	float r3 = urd_0_1(dre), g3 = urd_0_1(dre), b3 = urd_0_1(dre);
	float r4 = urd_0_1(dre), g4 = urd_0_1(dre), b4 = urd_0_1(dre);
	v1.color = glm::vec3(r1, g1, b1);
	v2.color = glm::vec3(r2, g2, b2);
	v3.color = glm::vec3(r3, g3, b3);
	v4.color = glm::vec3(r4, g4, b4);

	// Count : 1, Index: (0, 1, 2) // Count : 2, Index: (3, 4, 5) // Count : 3, Index: (6, 7, 8) ...
	int base_vertex = Vertex_glm_vec.size();
	size_t index_offset = index_vec.size();
	Vertex_glm_vec.push_back(v1); Vertex_glm_vec.push_back(v2); Vertex_glm_vec.push_back(v3); Vertex_glm_vec.push_back(v4);

	// two triangles
	shape.draw_mode = GL_TRIANGLES; shape.index_count = 6; shape.base_vertex = base_vertex;
	shape.index_offset = index_offset * sizeof(unsigned int);

	Current_Diagram_Count++;
	index_vec.push_back(0);
	index_vec.push_back(1);
	index_vec.push_back(3);
	index_vec.push_back(1);
	index_vec.push_back(2);
	index_vec.push_back(3);
	Model_vec.push_back(shape);

	std::cout << "Created Square at (" << x1 << ", " << y1 << "), (" << x2 << ", " << y2 << "), (" << x3 << ", " << y3 << "), (" << x4 << ", " << y4 << ")\n";
}

int PickObject(int x, int y) {
	// Picking ?åÎçîÎß??®Ïä§
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // Î∞∞Í≤Ω??0 (ID 0)?ºÎ°ú ?§Ï†ï
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(shaderProgramID);
	glBindVertexArray(VAO);

	// Picking Î™®Îìú ?úÏÑ±??
	GLint isPickingLoc = glGetUniformLocation(shaderProgramID, "u_IsPicking");
	GLint pickingColorLoc = glGetUniformLocation(shaderProgramID, "u_PickingColor");
	glUniform1i(isPickingLoc, GL_TRUE);

	for (int i = 0; i < Model_vec.size(); ++i) {
		Shape& shape = Model_vec[i];

		// Í∞ùÏ≤¥ ?∏Îç±??i+1)Î•?R, G, B ?âÏÉÅ?ºÎ°ú Î≥Ä??
		int r = (i + 1) & 0xFF;
		int g = ((i + 1) >> 8) & 0xFF;
		int b = ((i + 1) >> 16) & 0xFF;
		glUniform3f(pickingColorLoc, r / 255.0f, g / 255.0f, b / 255.0f);

		glDrawElementsBaseVertex(
			shape.draw_mode,
			shape.index_count,
			GL_UNSIGNED_INT,
			(void*)shape.index_offset,
			shape.base_vertex
		);
	}

	glFlush();
	glFinish();

	// ÎßàÏö∞???ÑÏπò???ΩÏ? ?âÏÉÅ ?ΩÍ∏∞
	unsigned char pixel[4];
	int inverted_y = glutGet(GLUT_WINDOW_HEIGHT) - y;
	glReadPixels(x, inverted_y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixel);

	// ?ΩÏ? ?âÏÉÅ???§Ïãú Í∞ùÏ≤¥ ?∏Îç±?§Î°ú Î≥Ä??
	int pickedID = pixel[0] + (pixel[1] << 8) + (pixel[2] << 16);

	// Picking Î™®Îìú ÎπÑÌôú?±Ìôî
	glUniform1i(isPickingLoc, GL_FALSE);

	if (pickedID == 0 || pickedID > Model_vec.size()) {
		return -1;
	}

	return pickedID - 1;
}

void Translation() {
	std::cout << "Translating selected object. Model num : " << selected_model_index << '\n';

	if (selected_model_index != -1 && moving_flag) {
		Shape& shape = Model_vec[selected_model_index];
		size_t index_start = shape.index_offset / sizeof(unsigned int);
		std::vector<bool> vertex_moved(Vertex_glm_vec.size(), false);

		glm::vec3 normalized_movement = movement_vec;
		float magnitude = glm::length(movement_vec);
		if (magnitude > 0.0f) {
			normalized_movement = glm::normalize(movement_vec) * movement_speed;
		}

		for (int i = 0; i < shape.index_count; ++i) {
			unsigned int local_index = index_vec[index_start + i];
			int actual_vertex_index = shape.base_vertex + local_index;

			if (actual_vertex_index < 0 || actual_vertex_index >= Vertex_glm_vec.size()) {
				std::cout << "Vertex index out of bounds: " << actual_vertex_index << ". Translation aborted.\n";
				return;
			}

			if (Vertex_glm_vec[actual_vertex_index].position.x + normalized_movement.x < -1.0f ||
				Vertex_glm_vec[actual_vertex_index].position.x + normalized_movement.x > 1.0f ||
				Vertex_glm_vec[actual_vertex_index].position.y + normalized_movement.y < -1.0f ||
				Vertex_glm_vec[actual_vertex_index].position.y + normalized_movement.y > 1.0f) {
				std::cout << "Translation would move vertex out of bounds: " << actual_vertex_index << ". Translation aborted.\n";
				return;
			}
		}

		for (int i = 0; i < shape.index_count; ++i) {
			unsigned int local_index = index_vec[index_start + i];
			int actual_vertex_index = shape.base_vertex + local_index;

			if (!vertex_moved[actual_vertex_index]) {
				Vertex_glm_vec[actual_vertex_index].position += normalized_movement;
				vertex_moved[actual_vertex_index] = true;
			}
		}
	}

	UPDATE_BUFFER();
	glutPostRedisplay();
}

void ChangeConsoleColor(int color) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, color);
}