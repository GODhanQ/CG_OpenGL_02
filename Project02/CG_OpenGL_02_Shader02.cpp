#define _CRT_SECURE_NO_WARNINGS
#include "VarANDFunc_02_Shader02.h"

auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
std::default_random_engine dre(seed);
std::uniform_real_distribution<float> urd_0_1(0.0f, 1.0f);
std::uniform_real_distribution<float> urd_m1_1(-1.0f, 1.0f);
std::uniform_real_distribution<float> urdRange(Triangle_range_Min, Triangle_range_Max);

GLuint shaderProgramID;
GLuint vertexShader;
GLuint fragmentShader;

// Index 0 : 1사분면, Index 1 : 2사분면, Index 2 : 3사분면, Index 3 :4 사분면
Shape models[4];

GLuint VAO, VBO, EBO;
DrawBatchManager drawBatchManager;

std::vector<Vertex_glm> Vertex_glm_vec;
std::vector<unsigned int> index_vec;

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | 0x0000);
	glutInitWindowPosition(500, 50);
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

	glBindVertexArray(VAO);

	// *** 최종 수정: 그리기 직전에 사용할 버퍼를 명시적으로 다시 바인딩합니다. ***
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	drawBatchManager.drawAll();

	glBindVertexArray(0);

	// 디버깅 로그는 그대로 둡니다.
	std::cout << "============ CURRENT STATE ============\n";
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
	std::cout << "============ INDEX & VERTEX END ============\n";

	glutSwapBuffers();
}

GLvoid Reshape(int w, int h) {
	glViewport(0, 0, w, h);
}

void KeyBoard(unsigned char key, int x, int y) {
	switch (key) {
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
		for(int i = 0; i < 4; i++) {
			models[i] = Shape(); // 모델 초기화
		}
		Vertex_glm_vec.clear();
		index_vec.clear();

		drawBatchManager.prepareDrawCalls(models);
		UPDATE_BUFFER();
		glutPostRedisplay();
		break;
	case 'q':
		exit(0);
	}
}

void MouseClick(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		if (drawing_type == NULL) {
			std::cout << "Please select a drawing mode first ( 'a' : Fill, 'b' : Line )\n";
			return;
		}

		std::pair<float, float> ogl_xy = ConvertScreenToOpenGL(x, y);
		float ogl_x{ ogl_xy.first }, ogl_y{ ogl_xy.second };
		std::cout << "Mouse Clicked at ( " << ogl_x << ", " << ogl_y << " )\n";

		int target_shape_index = -1;

		if (ogl_x >= 0.0f && ogl_y >= 0.0f) {
			target_shape_index = 0;
		}
		else if (ogl_x < 0.0f && ogl_y >= 0.0f) {
			target_shape_index = 1;
		}
		else if (ogl_x < 0.0f && ogl_y < 0.0f) {
			target_shape_index = 2;
		}
		else if (ogl_x >= 0.0f && ogl_y < 0.0f) {
			target_shape_index = 3;
		}

		if (target_shape_index != -1) {
			// 1. 현재 모델들의 상태(polygon_mode)를 임시 저장합니다.
			std::map<int, GLenum> old_polygon_modes;
			for (int i = 0; i < 4; ++i) {
				if (models[i].is_active) {
					old_polygon_modes[i] = models[i].polygon_mode;
				}
			}

			// 2. 기존 정점 제거
			auto it = Vertex_glm_vec.begin();
			while (it != Vertex_glm_vec.end()) {
				if (it->quadrant == target_shape_index) {
					it = Vertex_glm_vec.erase(it);
				}
				else {
					++it;
				}
			}

			// 3. 새로운 정점 데이터 생성
			glm::vec3 origin{ ogl_x, ogl_y, 0.0f };
			Vertex_glm v1, v2, v3;
			float top_ver_range = urdRange(dre);
			float left_bottom_range = urdRange(dre);
			float right_bottom_range = urdRange(dre);
			v1.position = origin + glm::vec3(0.0f, top_ver_range, 0.0f);
			v2.position = origin + glm::vec3(-left_bottom_range, -left_bottom_range, 0.0f);
			v3.position = origin + glm::vec3(right_bottom_range, -right_bottom_range, 0.0f);
			v1.color = glm::vec3(urd_0_1(dre), urd_0_1(dre), urd_0_1(dre));
			v2.color = glm::vec3(urd_0_1(dre), urd_0_1(dre), urd_0_1(dre));
			v3.color = glm::vec3(urd_0_1(dre), urd_0_1(dre), urd_0_1(dre));
			v1.quadrant = v2.quadrant = v3.quadrant = target_shape_index;

			// 4. 새로운 정점을 벡터에 추가
			Vertex_glm_vec.push_back(v1);
			Vertex_glm_vec.push_back(v2);
			Vertex_glm_vec.push_back(v3);

			// 5. 데이터를 사분면 순서로 정렬
			std::sort(Vertex_glm_vec.begin(), Vertex_glm_vec.end(), [](const Vertex_glm& a, const Vertex_glm& b) {
				return a.quadrant < b.quadrant;
				});

			// 6. 정렬된 최종 정점 목록을 기반으로 모든 것을 재구성
			index_vec.clear();
			int current_base_vertex = 0;
			for (int i = 0; i < 4; ++i) {
				models[i].is_active = false;

				bool shape_found_in_quadrant = false;
				for (const auto& vertex : Vertex_glm_vec) {
					if (vertex.quadrant == i) {
						shape_found_in_quadrant = true;
						break;
					}
				}

				if (shape_found_in_quadrant) {
					models[i].is_active = true;
					// *** 핵심 수정: polygon_mode를 올바르게 설정 ***
					if (i == target_shape_index) {
						// 새로 생성된 삼각형은 현재 drawing_type을 따름
						models[i].polygon_mode = drawing_type;
					}
					else {
						// 기존 삼각형은 이전에 저장해 둔 모드를 따름
						models[i].polygon_mode = old_polygon_modes[i];
					}
					models[i].base_vertex = current_base_vertex;
					models[i].index_offset = index_vec.size() * sizeof(unsigned int);
					models[i].index_start = index_vec.size();
					models[i].index_count = 3;

					index_vec.push_back(current_base_vertex);
					index_vec.push_back(current_base_vertex + 1);
					index_vec.push_back(current_base_vertex + 2);

					current_base_vertex += 3;
				}
			}
		}
		else {
			std::cout << "Error determining quadrant.\n";
			return;
		}

		drawBatchManager.prepareDrawCalls(models);
		UPDATE_BUFFER();
		glutPostRedisplay();
	}
}
std::pair<float, float> ConvertScreenToOpenGL(int screen_x, int screen_y) {
	float ogl_x = (2.0f * screen_x) / Window_width - 1.0f;
	float ogl_y = 1.0f - (2.0f * screen_y) / Window_height;
	return { ogl_x, ogl_y };
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
	//--- 버텍스 세이더 읽어 저장하고 컴파일 하기
	//--- filetobuf: 사용자정의 함수로 텍스트를 읽어서 문자열에 저장하는 함수
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
		std::cerr << "ERROR: vertex shader 컴파일 실패\n" << errorLog << std::endl;
		return;
	}
}


void make_fragmentShaders()
{
	GLchar* fragmentSource;
	//--- 프래그먼트 세이더 읽어 저장하고 컴파일하기
	fragmentSource = filetobuf("Fragment_Shader.glsl");	// 프래그세이더 읽어오기
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
	//glUseProgram(shaderID);							//--- 만들어진 세이더 프로그램 사용하기
	//--- 여러 개의 세이더프로그램 만들 수 있고, 그 중 한개의 프로그램을 사용하려면
	//--- glUseProgram 함수를 호출하여 사용 할 특정 프로그램을 지정한다.
	//--- 사용하기 직전에 호출할 수 있다.
	return shaderID;
}

void Make_Triangle(Shape& shape, float ogl_x, float ogl_y, int quardrant) {
	glm::vec3 origin{ ogl_x, ogl_y, 0.0f };
	Vertex_glm v1, v2, v3;

	float top_ver_range = urdRange(dre);
	float left_bottom_range = urdRange(dre);
	float right_bottom_range = urdRange(dre);

	v1.position = origin + glm::vec3(0.0f, top_ver_range, 0.0f);
	v2.position = origin + glm::vec3(-left_bottom_range, -left_bottom_range, 0.0f);
	v3.position = origin + glm::vec3(right_bottom_range, -right_bottom_range, 0.0f);
	
	v1.color = glm::vec3(urd_0_1(dre), urd_0_1(dre), urd_0_1(dre));
	v2.color = glm::vec3(urd_0_1(dre), urd_0_1(dre), urd_0_1(dre));
	v3.color = glm::vec3(urd_0_1(dre), urd_0_1(dre), urd_0_1(dre));

	v1.quadrant = quardrant; v2.quadrant = quardrant; v3.quadrant = quardrant;

	// 디버깅 정보
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
	
	// 현재 정점 인덱스를 기반으로 올바른 인덱스 계산
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
	std::cout << "Updating VBO, EBO \n";

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, Vertex_glm_vec.size() * sizeof(Vertex_glm), Vertex_glm_vec.data(), GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_vec.size() * sizeof(unsigned int), index_vec.data(), GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	std::cout << "Buffer Update Completed\n";
}
