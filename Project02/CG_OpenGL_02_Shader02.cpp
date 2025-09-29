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

// Index 0 : 1��и�, Index 1 : 2��и�, Index 2 : 3��и�, Index 3 :4 ��и�
Shape models[4];
Shape axis_model[2];

GLuint VAO_axis, VBO_axis, EBO_axis;
GLuint VAO, VBO, EBO;
DrawBatchManager drawBatchManager;
DrawAxisBatchManager drawAxisBatchManager;

std::vector<Vertex_glm> Axis_glm_vec;
std::vector<unsigned int> Axis_index_vec;

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

	glBindVertexArray(VAO_axis);
	drawAxisBatchManager.drawAll();
	glBindVertexArray(0);

	glBindVertexArray(VAO);

	// *** ���� ����: �׸��� ������ ����� ���۸� ��������� �ٽ� ���ε��մϴ�. ***
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	drawBatchManager.drawAll();

	glBindVertexArray(0);

	// ����� �α״� �״�� �Ӵϴ�.
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
			models[i] = Shape();
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
			std::map<int, GLenum> old_polygon_modes;
			for (int i = 0; i < 4; ++i) {
				if (models[i].is_active) {
					old_polygon_modes[i] = models[i].polygon_mode;
				}
			}

			auto it = Vertex_glm_vec.begin();
			while (it != Vertex_glm_vec.end()) {
				if (it->quadrant == target_shape_index) {
					it = Vertex_glm_vec.erase(it);
				}
				else {
					++it;
				}
			}

			glm::vec3 origin{ ogl_x, ogl_y, 0.0f };
			Vertex_glm v1, v2, v3;
			
			float triangle_range = urdRange(dre);
			

			/*float top_ver_range = urdRange(dre);
			float left_bottom_range = urdRange(dre);
			float right_bottom_range = urdRange(dre);*/
			v1.position = origin + glm::vec3(0.0f, 2.0 * triangle_range, 0.0f);
			v2.position = origin + glm::vec3(-triangle_range, -triangle_range, 0.0f);
			v3.position = origin + glm::vec3(triangle_range, -triangle_range, 0.0f);
			v1.color = glm::vec3(urd_0_1(dre), urd_0_1(dre), urd_0_1(dre));
			v2.color = glm::vec3(urd_0_1(dre), urd_0_1(dre), urd_0_1(dre));
			v3.color = glm::vec3(urd_0_1(dre), urd_0_1(dre), urd_0_1(dre));
			v1.quadrant = v2.quadrant = v3.quadrant = target_shape_index;

			Vertex_glm_vec.push_back(v1);
			Vertex_glm_vec.push_back(v2);
			Vertex_glm_vec.push_back(v3);

			std::sort(Vertex_glm_vec.begin(), Vertex_glm_vec.end(), [](const Vertex_glm& a, const Vertex_glm& b) {
				return a.quadrant < b.quadrant;
				});

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
					if (i == target_shape_index) {
						models[i].polygon_mode = drawing_type;
					}
					else {
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
	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
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

		// save polygon modes
		if (target_shape_index != -1) {
			std::map<int, GLenum> old_polygon_modes;
			for (int i = 0; i < 4; ++i) {
				if (models[i].is_active) {
					old_polygon_modes[i] = models[i].polygon_mode;
				}
			}
			
			// save old colors
			std::vector<glm::vec3> old_color;
			Shape& target_shape = models[target_shape_index];
			old_color.push_back(Vertex_glm_vec[target_shape.index_start].color);
			old_color.push_back(Vertex_glm_vec[target_shape.index_start + 1].color);
			old_color.push_back(Vertex_glm_vec[target_shape.index_start + 2].color);

			// erase vertices in the target quadrant
			auto it = Vertex_glm_vec.begin();
			while (it != Vertex_glm_vec.end()) {
				if (it->quadrant == target_shape_index) {
					it = Vertex_glm_vec.erase(it);
				}
				else {
					++it;
				}
			}

			// 3. restting the triangle
			glm::vec3 origin{ ogl_x, ogl_y, 0.0f };
			Vertex_glm v1, v2, v3;

			float triangle_range = urdRange(dre);
			/*float top_ver_range = urdRange(dre);
			float left_bottom_range = urdRange(dre);
			float right_bottom_range = urdRange(dre);*/
			v1.position = origin + glm::vec3(0.0f, triangle_range, 0.0f);
			v2.position = origin + glm::vec3(-triangle_range, -triangle_range, 0.0f);
			v3.position = origin + glm::vec3(triangle_range, -triangle_range, 0.0f);
			v1.color = old_color[0];
			v2.color = old_color[1];
			v3.color = old_color[2];
			v1.quadrant = v2.quadrant = v3.quadrant = target_shape_index;

			Vertex_glm_vec.push_back(v1);
			Vertex_glm_vec.push_back(v2);
			Vertex_glm_vec.push_back(v3);

			std::sort(Vertex_glm_vec.begin(), Vertex_glm_vec.end(), [](const Vertex_glm& a, const Vertex_glm& b) {
				return a.quadrant < b.quadrant;
				});

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
					if (i == target_shape_index) {
						models[i].polygon_mode = drawing_type;
					}
					else {
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

void Make_Triangle(Shape& shape, float ogl_x, float ogl_y, int quardrant) {
	glm::vec3 origin{ ogl_x, ogl_y, 0.0f };
	Vertex_glm v1, v2, v3;

	float triangle_range = urdRange(dre);
	/*float top_ver_range = urdRange(dre);
	float left_bottom_range = urdRange(dre);
	float right_bottom_range = urdRange(dre);*/

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

void INIT_BUFFER()
{
	std::cout << "Initializing VAO, EBO \n";
	float axis_color1 = urd_0_1(dre), axis_color2 = urd_0_1(dre);
	float axis_vertex[] = {
		// X axis
		-1.0f, 0.0f, 0.0f,	axis_color1, axis_color2, axis_color1,
		 1.0f, 0.0f, 0.0f,	axis_color1, axis_color2, axis_color1,
		 // Y axis
		  0.0f,-1.0f, 0.0f,	axis_color2, axis_color1, axis_color2,
		  0.0f, 1.0f, 0.0f,	axis_color2, axis_color1, axis_color2
	};
	unsigned int axis_index[] = {
		0, 1,
		2, 3
	};
	for (int i = 0; i < 4; ++i) {
		Vertex_glm v;
		v.position = glm::vec3(axis_vertex[i * 6], axis_vertex[i * 6 + 1], axis_vertex[i * 6 + 2]);
		v.color = glm::vec3(axis_vertex[i * 6 + 3], axis_vertex[i * 6 + 4], axis_vertex[i * 6 + 5]);
		v.quadrant = -1;
		Axis_glm_vec.push_back(v);
	}
	for (int i = 0; i < 4; ++i) {
		Axis_index_vec.push_back(axis_index[i]);
	}

	for (int i = 0; i < 2; ++i) {
		axis_model[i].draw_mode = GL_LINES;
		axis_model[i].polygon_mode = GL_LINE;
		axis_model[i].index_count = 2;
		axis_model[i].base_vertex = i * 2;
		axis_model[i].index_offset = i * 2 * sizeof(unsigned int);
		axis_model[i].index_start = i * 2;
		axis_model[i].is_active = true;
	}

	glGenVertexArrays(1, &VAO_axis);
	glGenBuffers(1, &VBO_axis);
	glGenBuffers(1, &EBO_axis);

	glBindVertexArray(VAO_axis);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_axis);
	glBufferData(GL_ARRAY_BUFFER, Axis_glm_vec.size() * sizeof(Vertex_glm), Axis_glm_vec.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_glm), (void*)offsetof(Vertex_glm, position));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_glm), (void*)offsetof(Vertex_glm, color));
	glEnableVertexAttribArray(1);
	glVertexAttribIPointer(2, 1, GL_INT, sizeof(Vertex_glm), (void*)offsetof(Vertex_glm, quadrant));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_axis);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, Axis_index_vec.size() * sizeof(unsigned int), Axis_index_vec.data(), GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	drawAxisBatchManager.prepareDrawCalls(axis_model);
	// --------------------------------------------------------
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
