#ifdef _WIN32

#pragma comment(lib, "opengl32.lib")
//For resize
#pragma comment(lib,"glu32.lib")
#pragma comment(lib, "freeglut.lib")

#ifdef GLEW_STATIC
#pragma comment(lib, "glew32s.lib")
#else
#pragma comment(lib, "glew32.lib")
#endif
#include <Windows.h>
#define FREEGLUT_LIB_PRAGMAS 0
#endif

#include <cstdio>
#include <cmath>
#include "GL/glew.h"
#include "GL/freeglut.h"

#include "../common/EsgiShader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include <vector>

EsgiShader basic;
GLuint textureID;
GLuint VBO;
GLuint IBO;
GLuint VAO;

struct point {
	std::vector<float> position;
	std::vector<float> texcoords;
	std::vector<float> normale;
};

std::vector<float> positions = {};
std::vector<float> texcoords;
std::vector<float> normales;

std::vector<float> points = { 1.000000f, -1.000000f, -1.000000f, 0.748573f, 0.750412f,
1.000000f, -1.000000f, 1.000000f, 0.749279f, 0.501284f,
-1.000000f, -1.000000f, 1.000000f, 1.0f, 0.0f,
-1.000000f, -1.000000f, -1.000000f, 1.0f, 1.0f,
1.000000f, 1.000000f, -1.000000f, 0.749279f, 0.501284f,
0.999999f, 1.000000f, 1.000001f, 0.0f, 1.0f,
-1.000000f, 1.000000f, -1.000000f, 1.0f, 0.0f };
std::vector<GLushort> indices = { 5, 1, 4,
								5, 4, 8,
								3, 7, 8,
								3, 8, 4,
								2, 6, 3,
								6, 7, 3,
								1,5,2,
								5,6,2};
std::vector<GLuint> listOfReadedPoint;

GLuint CreateTexture(const char* nom)
{
	int w, h, comp;
	int req_comp = 4;
	auto* image = stbi_load(nom, &w, &h, &comp, req_comp);

	GLuint textureObj;
	glGenTextures(1, &textureObj);
	glBindTexture(GL_TEXTURE_2D, textureObj);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(image);

	glBindTexture(GL_TEXTURE_2D, 0);

	return textureObj;
}

void DestroyTexture(GLuint textureObj)
{
	glDeleteTextures(1, &textureObj);
}

void Initialize()
{
	glewInit();
	basic.LoadVertexShader("shader.vs");
	basic.LoadFragmentShader("shader.fs");
	basic.Create();

	textureID = CreateTexture("Koala.jpg");

	//pointArray
	/*const float carre[] = {
		-0.5f, 0.5f, 0.0f, 0.0f,	
		-0.5f, -0.5f, 0.0f, 1.0f,	
		0.5f, 0.5f, 1.0f, 0.0f,		
		0.5f, -0.5f, 1.0f, 1.0f		
	};*/

	/*int index = 0;
	for (GLushort i = 0; i < 12; i++)
	{
		for (GLushort j = 0; j < 3; j++)
		{
			bool vertextExist = false;
			int nbReadForV = 5;
			int nbReadForVT = 1;
			int nbReadForVN = 1;
			index = 0;
			GLushort k;
			for ( k = 0; k < indices.size()-3; k+=3)
			{
				if (nbReadForV == indices[k] && nbReadForVT == indices[k + 1] && nbReadForVN == indices[k + 2]) {
					vertextExist = true;
					break;
				}
			}
			if (!vertextExist) {
				points.push_back(positions[(nbReadForV - 1) * 3]);
				points.push_back(positions[(nbReadForV - 1) * 3 + 1]);
				points.push_back(positions[(nbReadForV - 1) * 3 + 2]);

				points.push_back(texcoords[(nbReadForVT - 1) * 2]);
				points.push_back(texcoords[(nbReadForVT - 1) * 2 + 1]);

				points.push_back(normales[(nbReadForVN - 1) * 3]);
				points.push_back(normales[(nbReadForVN - 1) * 3 + 1]);
				points.push_back(normales[(nbReadForVN - 1) * 3 + 2]);

				listOfReadedPoint.push_back(nbReadForV);
				listOfReadedPoint.push_back(nbReadForVT);
				listOfReadedPoint.push_back(nbReadForVN);

				indices.push_back(index++);
			}
			else
			{
				indices.push_back(k/3);
			}
		}
	}*/

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * points.size()/** 5 * 8*/, /*carre*/ &points[0], GL_STATIC_DRAW);

	//const GLushort indices[] = { 0, 1, 2, 3 };

	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * indices.size(), &indices[0] /*indices*/, GL_STATIC_DRAW);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	auto program = basic.GetProgram();
	glUseProgram(program);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	auto positionAttrib = glGetAttribLocation(program, "a_position");
	glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, false, sizeof(float) * 4, 0);
	glEnableVertexAttribArray(positionAttrib);
	auto texcoordsAttrib = glGetAttribLocation(program, "a_texcoords");
	glVertexAttribPointer(texcoordsAttrib, 2, GL_FLOAT, false, sizeof(float) * 4, (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(texcoordsAttrib);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Terminate()
{
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &IBO);
	DestroyTexture(textureID);
	basic.Destroy();
}

void Resize(int width, int height)
{
	glViewport(0, 0, width, height);
	//glMatrixMode(GL_PROJECTION);
	//glu32
	//gluPerspective(45, (float)width / (float)height, 1.5, 20);
}

void Update()
{
	glutPostRedisplay();
}

void Render()
{
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);

	auto program = basic.GetProgram();
	glUseProgram(program);

	const float scaleMatrix[]{
		1.f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.f,0.0f, 0.0f,
		0.0f, 0.0f, 1.f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	auto scaleLocation = glGetUniformLocation(program, "u_scaleMatrix");
	glUniformMatrix4fv(scaleLocation, 1, GL_FALSE, scaleMatrix);

	float time = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
	const float rotationMatrix[]{
		cos(time), -sin(time), 0.0f, 0.0f,
		-sin(time), cos(time),0.0f, 0.0f,
		0.0f, 0.0f, 1.f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	auto rotationLocation = glGetUniformLocation(program, "u_rotationMatrix");
	glUniformMatrix4fv(rotationLocation, 1, GL_FALSE, rotationMatrix);

	float r = glutGet(GLUT_WINDOW_WIDTH) * 2.0f;
	float l = -r;//0.0f
	float t = glutGet(GLUT_WINDOW_HEIGHT) / 2.0f;
	float b = -t;//0.0f
	float n = -1.f;
	float f = 1.f;
	const float projectionMatrix[]{
		2.f / (r - 1), 0.0f, 0.0f, 0.0f,
		0.0f, 2.0f / (t - b),0.0f, 0.0f,
		0.0f, 0.0f, 2.f / (f - n), 0.0f,
		-(r + 1) / (r - 1), -(t + b) / (t - b), -(f + n) / (f - n), 1.0f
	};

	auto projLocation = glGetUniformLocation(program, "u_projectionMatrix");
	glUniformMatrix4fv(projLocation, 1, GL_FALSE, projectionMatrix);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);
	auto textureLoc0 = glGetUniformLocation(program, "u_texture0");
	glUniform1i(textureLoc0, 0);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 8, GL_UNSIGNED_SHORT, (void*)0);

	glBindVertexArray(0);

	glutSwapBuffers();
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	glutCreateWindow("ObjLoader");

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	GLfloat posLight0[] = { 0.0f,0.0f,-1.0f,0.0f };
	GLfloat lightColor[] = { 1.0f,1.0f,1.0f,1.0f };

	glLightfv(GL_LIGHT0, GL_POSITION, posLight0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);

	Initialize();

	glutReshapeFunc(Resize);
	glutIdleFunc(Update);
	glutDisplayFunc(Render);
	glutMainLoop();

	Terminate();

	return 0;
}