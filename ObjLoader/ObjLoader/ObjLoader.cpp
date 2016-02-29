// CarreMultiTextures.cpp : définit le point d'entrée pour l'application console.
//

// force l'utilisation du linkage static
//#define GLEW_STATIC

#ifdef _WIN32
#pragma comment(lib, "opengl32.lib")
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

//use_CRT_SECURE_NO_WARNINGS;
#include "../common/EsgiShader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include "ObjectLoader.h"

EsgiShader basic;

void Initialize()
{
	auto program = basic.GetProgram();
}

void Terminate()
{
	basic.Destroy();
}

void Resize(int width, int height)
{
	glViewport(0, 0, width, height);
}

void Update()
{
	glutPostRedisplay();
}

void Render()
{
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
}

int main(int argc, char* argv[])
{
	ObjectLoader obi = ObjectLoader();

	vector<float> i = obi.loadElements("MontExample1.obj", "f");

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(800, 600);
	glutCreateWindow("ObjLoader");

	Initialize();

	glutReshapeFunc(Resize);
	glutIdleFunc(Update);
	glutDisplayFunc(Render);
	glutMainLoop();

	Terminate();

	return 0;
}