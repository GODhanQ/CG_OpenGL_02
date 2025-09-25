#define _CRT_SECURE_NO_WARNINGS
#include "VarANDFunc_02_Shader01.h"

auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
std::default_random_engine dre(seed);
std::uniform_real_distribution<float> urd_0_1(0.0f, 1.0f);
std::uniform_real_distribution<float> urd_m1_1(-1.0f, 1.0f);

GLuint shaderProgramID;
GLuint vertexShader;
GLuint fragmentShader;

GLuint VAO, VBO_pos, VBO_color, VBO, EBO;

// Vertex Data : position(x,y,z), color(r,g,b)
std::vector<Vertex_glm> Vertex_glm_vec;
// Vertex Index (Triangle)
std::vector<unsigned int> index_vec;
// Shape models : Shape
std::vector<Shape> Model_vec;

DrawBatchManager drawBatchManager;
int active_line_strip_model_index = -1;

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

	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(MouseClick);

	glutMainLoop();
}

GLvoid drawScene() {
	GLfloat rColor{ 0.3f }, gColor{ 0.3f }, bColor{ 0.3f };
	glClearColor(rColor, gColor, bColor, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(shaderProgramID);
	if (!index_vec.empty()) {
		glBindVertexArray(VAO);
		std::cout << "Binding VAO and Drawing Elements with count: " << index_vec.size() << "\n";

		// the whole object Must be drawn separately, if i make a one point, and one triangle
		// it should be drawn separately
		// current it does not acting like that

		glPointSize(10.0);
		glLineWidth(5.0);

		drawBatchManager.prepareDrawCalls(Model_vec);
		drawBatchManager.drawAll();
		std::cout << "Current Object Count: " << Model_vec.size() << "\n";
		/*for (auto& shape : Model_vec) {
			std::cout << "Draw Mode: " << shape.draw_mode << ", Index Count: " << shape.index_count
				<< ", Base Vertex: " << shape.base_vertex << ", Index Offset (bytes): " << shape.index_offset << "\n";
			std::cout << "\nVertex Position and Color:\n";
			for (int i = 0; i < shape.index_count; i++) {
				int vertex_index = index_vec[shape.base_vertex + i];
				if (vertex_index < Vertex_glm_vec.size()) {
					auto& vertex = Vertex_glm_vec[vertex_index];
					std::cout << "  Vertex " << vertex_index << ": Position(" 
						<< vertex.position.x << ", " << vertex.position.y << ", " << vertex.position.z 
						<< "), Color(" << vertex.color.r << ", " << vertex.color.g << ", " << vertex.color.b << ")\n";
				}
			}
			std::cout << "-----------------------------------\n";
		}*/

		/*
		if (DrawPoint_mode) {
			glDrawElements(GL_POINTS, index_vec.size(), GL_UNSIGNED_INT, 0);
		}
		else if (DrawLine_mode) {
			glDrawElements(GL_LINES, index_vec.size(), GL_UNSIGNED_INT, 0);
		}
		else if (DrawTriangle_mode)
			glDrawElements(GL_TRIANGLES, index_vec.size(), GL_UNSIGNED_INT, 0);
		else if (DrawSquare_mode)
			// i want to draw suare with two triangles but the square is sperated other squares
			// so i will use triangle strip to draw square with two triangles
			glDrawElements(GL_TRIANGLES, index_vec.size(), GL_UNSIGNED_INT, 0);
		*/

		//glDrawElements(GL_TRIANGLES, index_vec.size(), GL_UNSIGNED_INT, 0);
		//glDrawElements(GL_TRIANGLES, 6 GL_UNSIGNED_INT, 0);
		//glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, 0);
		//glBindVertexArray(0);
	}
	
	//glPointSize(10.0);
	//glDrawArrays(GL_TRIANGLES, 0, 3);
	//glDrawArrays(GL_QUADS, 0, 4);

	glutSwapBuffers();
}

GLvoid Reshape(int w, int h) {
	glViewport(0, 0, w, h);
}

void Keyboard(unsigned char key, int x, int y) {
	// 모드 변경 시, 진행 중이던 라인 스트립 그리기를 종료
	if (key != 'l') { // 'l' 키를 누른 게 아니라면 라인 그리기 상태 초기화
		active_line_strip_model_index = -1;
	}

	switch (key) {
	case 'p':
		if (DrawLine_mode) { // 라인 모드가 켜져 있으면 다른 모드 선택 불가
			std::cout << "Please deactivate Line Drawing Mode ('l') first.\n";
			break;
		}
		if (DrawTriangle_mode || DrawSquare_mode) {
			std::cout << "Another drawing mode is already selected. Please deselect it first.\n";
			break;
		}
		DrawPoint_mode = !DrawPoint_mode;
		
		std::cout << "Point Drawing Mode :" << (DrawPoint_mode ? " On" : " Off") << '\n';
		break;
	case 'l':
		if (DrawPoint_mode || DrawTriangle_mode || DrawSquare_mode) {
			std::cout << "Another drawing mode is already selected. Please deselect it first.\n";
			break;
		}
		DrawLine_mode = !DrawLine_mode;
		// 라인 모드가 꺼지면 그리던 선을 확정
		if (!DrawLine_mode) {
			active_line_strip_model_index = -1;
		}

		std::cout << "Line Drawing Mode :" << (DrawLine_mode ? " On" : " Off") << '\n';
		break;
	case 't':
		if (DrawLine_mode) { // 라인 모드가 켜져 있으면 다른 모드 선택 불가
			std::cout << "Please deactivate Line Drawing Mode ('l') first.\n";
			break;
		}
		if (DrawPoint_mode || DrawSquare_mode) {
			std::cout << "Another drawing mode is already selected. Please deselect it first.\n";
			break;
		}
		DrawTriangle_mode = !DrawTriangle_mode;

		std::cout << "Triangle Drawing Mode :" << (DrawTriangle_mode ? " On" : " Off") << '\n';
		break;
	case 's':
		if (DrawLine_mode) { // 라인 모드가 켜져 있으면 다른 모드 선택 불가
			std::cout << "Please deactivate Line Drawing Mode ('l') first.\n";
			break;
		}
		if (DrawPoint_mode || DrawTriangle_mode) {
			std::cout << "Another drawing mode is already selected. Please deselect it first.\n";
			break;
		}
		DrawSquare_mode = !DrawSquare_mode;

		std::cout << "Square Drawing Mode :" << (DrawSquare_mode ? " On" : " Off") << '\n';
		break;
	case 'q':
		exit(0);
	}
}

void MouseClick(int button , int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		if (Current_Diagram_Count >= 10 && active_line_strip_model_index == -1) { // 새 도형을 만들 때만 개수 체크
			std::cout << "Maximum number of Diagram reached (10). Cannot add more.\n";
			return;
		}
		std::pair<float, float> ogl_xy = ConvertMouseWxy2GLxy(x, y);
		float ogl_x = ogl_xy.first;
		float ogl_y = ogl_xy.second;
		std::cout << "Mouse Clicked at ( " << ogl_x << ", " << ogl_y << " )\n";

		if (DrawPoint_mode) {
			active_line_strip_model_index = -1; // 다른 모드를 클릭하면 라인 그리기 종료
			CreatePointAtOrigin(ogl_x, ogl_y);
		}
		else if (DrawLine_mode) {
			AddVertexToLineStrip(ogl_x, ogl_y);
		}
		else if (DrawTriangle_mode) {
			active_line_strip_model_index = -1; // 다른 모드를 클릭하면 라인 그리기 종료
			CreateTriangleAtOrigin(ogl_x, ogl_y);
		}
		else if (DrawSquare_mode) {
			active_line_strip_model_index = -1; // 다른 모드를 클릭하면 라인 그리기 종료
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

// uniform 변수는 무조건 할당받기
// 정점 위치 바뀌면 버퍼에 다시 보내주기

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
	//--- 버텍스 세이더 읽어 저장하고 컴파일 하기
	//--- filetobuf: 사용자정의 함수로 텍스트를 읽어서 문자열에 저장하는 함수
	vertexSource = filetobuf("vertex.glsl");
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		std::cerr << "ERROR: vertex shader 컴파일 실패\n" << errorLog << std::endl;
		return;
	}
}


void make_fragmentShaders()
{
	GLchar* fragmentSource;
	//--- 프래그먼트 세이더 읽어 저장하고 컴파일하기
	fragmentSource = filetobuf("fragment.glsl");	// 프래그세이더 읽어오기
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		std::cerr << "ERROR: frag_shader 컴파일 실패\n" << errorLog << std::endl;
		return;
	}
}

GLuint make_shaderProgram()
{
	GLint result;
	GLchar* errorLog = NULL;
	GLuint shaderID;
	shaderID = glCreateProgram();					//--- 세이더 프로그램 만들기
	glAttachShader(shaderID, vertexShader);			//--- 세이더 프로그램에 버텍스 세이더 붙이기
	glAttachShader(shaderID, fragmentShader);		//--- 세이더 프로그램에 프래그먼트 세이더 붙이기
	glLinkProgram(shaderID);						//--- 세이더 프로그램 링크하기
	glDeleteShader(vertexShader);					//--- 세이더 객체를 세이더 프로그램에 링크했음으로, 세이더 객체 자체는 삭제 가능
	glDeleteShader(fragmentShader);
	glGetProgramiv(shaderID, GL_LINK_STATUS, &result); // ---세이더가 잘 연결되었는지 체크하기
	if (!result) {
		glGetProgramInfoLog(shaderID, 512, NULL, errorLog);
		std::cerr << "ERROR: shader program 연결 실패\n" << errorLog << std::endl;
		return false;
	}
	glUseProgram(shaderID);							//--- 만들어진 세이더 프로그램 사용하기
	//--- 여러 개의 세이더프로그램 만들 수 있고, 그 중 한개의 프로그램을 사용하려면
	//--- glUseProgram 함수를 호출하여 사용 할 특정 프로그램을 지정한다.
	//--- 사용하기 직전에 호출할 수 있다.
	return shaderID;
}

void INIT_BUFFER()
{
	std::cout << "Initializing VAO, EBO \n";

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	//glGenBuffers(1, &VBO_pos);
	//glGenBuffers(1, &VBO_color);

	/*
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_pos);
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(vPositionList[0]), vPositionList, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_color);
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(vColorList[0]), vColorList, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	*/

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_glm), (void*)offsetof(Vertex_glm, position));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_glm), (void*)offsetof(Vertex_glm, color));
	
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
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

	// 버퍼 바인딩 해제
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
	// 새 정점 생성
	Vertex_glm v;
	v.position = glm::vec3(ogl_x, ogl_y, 0.0f);
	v.color = glm::vec3(urd_0_1(dre), urd_0_1(dre), urd_0_1(dre));

	if (active_line_strip_model_index == -1) {
		// --- 새로운 라인 스트립 시작 ---
		Shape shape;
		shape.draw_mode = GL_LINE_STRIP;
		shape.index_count = 1; // 시작은 정점 1개
		shape.base_vertex = Vertex_glm_vec.size();
		shape.index_offset = index_vec.size() * sizeof(unsigned int);

		Vertex_glm_vec.push_back(v);
		index_vec.push_back(0); // 로컬 인덱스 0

		Model_vec.push_back(shape);
		active_line_strip_model_index = Model_vec.size() - 1; // 새로 만든 Shape의 인덱스 저장
		Current_Diagram_Count++;
		std::cout << "Started a new Line Strip.\n";
	}
	else {
		// --- 기존 라인 스트립에 정점 추가 ---
		Shape& shape = Model_vec[active_line_strip_model_index];

		// 새 정점을 전체 정점 벡터의 맨 뒤에 추가
		Vertex_glm_vec.push_back(v);
		
		// 새 인덱스를 전체 인덱스 벡터의 맨 뒤에 추가
		// 로컬 인덱스는 현재 shape이 가진 인덱스 개수와 동일
		index_vec.push_back(shape.index_count); 
		
		// 이 Shape을 그릴 때 사용할 인덱스 개수 1 증가
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
