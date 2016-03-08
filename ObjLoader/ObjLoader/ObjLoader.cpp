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

//Includes necessaires
#include <cstdio>
#include <cmath>
#include "GL/glew.h"
#include "GL/freeglut.h"

// Le shader
#include "../common/EsgiShader.h"

// Pour le traitement de la texture
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

// Pour les tableaux
#include <vector>

#include "ObjectLoader.h"

using namespace std;

EsgiShader basic;
// Identifiant de la texture utilsée
GLuint textureID;
// En rapport avec les positions des sommets
GLuint VBO;
// En rapport avec les indices
GLuint IBO;
// Manager des VBO
GLuint VAO;

/*struct point {
	std::vector<float> position;
	std::vector<float> texcoords;
	std::vector<float> normale;
};*/

//Structure remplis par la premiére lecture du obj
std::vector<float> positions = {};
std::vector<float> texcoords = {};
std::vector<float> normales = {};

// Tableau final avec touts les élements de l'obj, sauf la forme: v1, v2, v3, vt1, vt2, vn1, vn2, vn3, v1...
// On aura un seul VBO
/*std::vector<float> points = { 1.000000f, -1.000000f, -1.000000f, 0.748573f, 0.750412f,
1.000000f, -1.000000f, 1.000000f, 0.749279f, 0.501284f,
-1.000000f, -1.000000f, 1.000000f, 1.0f, 0.0f,
-1.000000f, -1.000000f, -1.000000f, 1.0f, 1.0f,
1.000000f, 1.000000f, -1.000000f, 0.749279f, 0.501284f,
0.999999f, 1.000000f, 1.000001f, 0.0f, 1.0f,
-1.000000f, 1.000000f, -1.000000f, 1.0f, 0.0f };*/
/*std::vector<float> points = {
-0.5f, -0.5f, 0.5f,
0.5f, -0.5f, 0.5f,
0.5f, 0.5f, 0.5f,
-0.5f, 0.5f, 0.5f,

0.5f, -0.5f, -0.5f,
0.5f, 0.5f, -0.5f,
-0.5f, -0.5f, -0.5f,
-0.5f, 0.5f, -0.5f
};*/

ObjectLoader ob = ObjectLoader();

std::vector<float> points = {
	-0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
	-0.5f, -0.5f, 0.5f, 0.0f, 1.0f,
	0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
	0.5f, -0.5f, 0.5f, 1.0f, 1.0f,
	- 0.5f, 0.5f, -0.5f, 0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
	0.5f, 0.5f, -0.5f, 1.0f, 0.0f,
	0.5f, -0.5f, -0.5f, 1.0f, 1.0f
};

const float Texture[]{
	0.0f, 0.0f,
	0.0f, 1.f,
	1.0f, 0.f,
	1.0f, 1.0f
};

// Tableau final des indices qui se remplie au fil de l'algo avec des indices de sommets de "points" 
/*std::vector<GLushort> indices = { 5, 1, 4,
								5, 4, 8,
								3, 7, 8,
								3, 8, 4,
								2, 6, 3,
								6, 7, 3,
								1,5,2,
								5,6,2,
5,8,6,
8,7,6,1,2,3,1,3,4};*/
//std::vector<GLushort> indices = { 0,1,2,0,2,3, 1,4,5,1,5,2 ,0,7,6,0,3,7, 0,4,1,0,6,4, 6,5,4,6,7,5 };
std::vector<GLushort> indices = { 0, 1, 2, 1, 3, 2,  1,5,3,5,7,3,  2,3,6,3,7,6,  2,6,0,6,4,0,  0,4,1,4,5,1,  6,7,4,7,5,4 };
//std::vector<GLushort> indices = { 0,2,1,0,3,2, 1,5,4,1,2,5 ,0,7,6,0,3,7, 0,8,1,0,9,8, 9,5,8,9,7,5 };

// Liste des sommets déja lut (combinaisont de v/vt/vn)
std::vector<GLuint> listOfReadedPoint;

int texH, texW;

// FBO pour la partie 2
struct Framebuffer
{
	GLuint ColorTex0;
	GLuint FBO;

	bool CreateFBO(GLuint w, GLuint h);
	void DestroyFBO();
};

// FBO partie 2
Framebuffer g_FBO;
GLuint CreateEmptyTexture(GLuint w, GLuint h)
{
	GLuint textureObj;
	glGenTextures(1, &textureObj);
	glBindTexture(GL_TEXTURE_2D, textureObj);
	//rgb4 in 2.0
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	return textureObj;
}
bool Framebuffer::CreateFBO(GLuint w, GLuint h)
{
	ColorTex0 = CreateEmptyTexture(w, h);
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ColorTex0, 0);
	GLenum check = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return (check == GL_FRAMEBUFFER_COMPLETE);
}
void Framebuffer::DestroyFBO()
{
	glDeleteFramebuffers(1, &FBO);
	glDeleteTextures(1, &ColorTex0);
}

GLuint CreateTexture(const char* nom)
{
	int w, h, comp;
	int req_comp = 4;
	auto* image = stbi_load(nom, &w, &h, &comp, req_comp);
	texH = h;
	texW = w;

	GLuint textureObj;
	//Demander de l'identifiant
	glGenTextures(1, &textureObj);
	glBindTexture(GL_TEXTURE_2D, textureObj);
	// Allocation
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	// Paramétres de la texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(image);

	//Dessiner sans texture pour le moment
	glBindTexture(GL_TEXTURE_2D, 0);

	return textureObj;
}

void DestroyTexture(GLuint textureObj)
{
	glDeleteTextures(1, &textureObj);
}

void Initialize()
{
	
	points = ob.elements;
	indices = ob.getIndicesToGLushort();
	glewInit();
	basic.LoadVertexShader("shader.vs");
	basic.LoadFragmentShader("shader.fs");
	basic.Create();

	// On récupére l'identifiant de la texture créée
	textureID = CreateTexture("Koala.jpg");

	// Création
	glGenBuffers(1, &VBO);
	// Définiti comme actif
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// Allocation
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * points.size()/** 5 * 8*/, /*carre*/ &points[0], GL_STATIC_DRAW);

	// Création
	glGenBuffers(1, &IBO);
	// Définiti comme actif
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	// Allocation
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * indices.size(), &indices[0] /*indices*/, GL_STATIC_DRAW);

	auto program = basic.GetProgram();
	glUseProgram(program);

	// On crée le VAO
	glGenVertexArrays(1, &VAO);
	// Qui est alors défini comme actif
	glBindVertexArray(VAO);

	// Necessaire mais n'est pas stocké dans le VAO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	auto positionAttrib = glGetAttribLocation(program, "a_position");
	// Adresse relative dans le VBO
	glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, false, sizeof(float) * 8, 0);
	glEnableVertexAttribArray(positionAttrib);
	auto texcoordsAttrib = glGetAttribLocation(program, "a_texcoords");
	//glVertexAttribPointer(texcoordsAttrib, 2, GL_FLOAT, false, sizeof(float) * 2, /*(void*)(3 * sizeof(float))*/Texture);
	glVertexAttribPointer(texcoordsAttrib, 2, GL_FLOAT, false, sizeof(float) * 8, (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(texcoordsAttrib);

	
	auto normalAttrib = glGetAttribLocation(program, "a_normal");

	glVertexAttribPointer(normalAttrib, 3, GL_FLOAT, false, sizeof(float) * 8, (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(normalAttrib);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	// Ne rend plus actif
	// Permet de desactiver le VAO
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// Désactive ce dont on ne se sert plus
	//glEnableVertexAttribArray(0);

	bool valid = g_FBO.CreateFBO(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	//bool valid = g_FBO.CreateFBO(texW, texH);
	assert(valid == true);
}

void Terminate()
{
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &IBO);

	g_FBO.DestroyFBO();

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
	// Pour la partie 2
	glBindFramebuffer(GL_FRAMEBUFFER, g_FBO.FBO);

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	auto program = basic.GetProgram();
	glUseProgram(program);

	// Matrices
	const float scaleMatrix[]{
		0.005f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.005f,0.0f, 0.0f,
		0.0f, 0.0f, 0.005f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
	auto scaleLocation = glGetUniformLocation(program, "u_scaleMatrix");
	glUniformMatrix4fv(scaleLocation, 1, GL_FALSE, scaleMatrix);

	float time = glutGet(GLUT_ELAPSED_TIME) / 5000.0f;
	const float rotationMatrix[]{
		cos(time), sin(time), 0.0f, 0.0f,
		-sin(time), cos(time),0.0f, 0.0f,
		0.0f, cos(time), -sin(time), 0.0f,
		//0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
	/*const float rotationMatrix[]
	{1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, cos(time), sin(time), 0.0f,
		0.0f, -sin(time), cos(time), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f};*/
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

	//Rendre actif sur l'unité de texture 0
	glActiveTexture(GL_TEXTURE0);
	// Active la texture correspondante é cet id
	glBindTexture(GL_TEXTURE_2D, textureID);
	// Récupére la localisation mémoire de la variable
	auto textureLoc0 = glGetUniformLocation(program, "u_texture0");
	// Set la variable
	glUniform1i(textureLoc0, 0);

	/*static const float triangle[] = {
		1.0, 0.0, 0.0, 1.0,
		0.0, 1.0, 0.0, 1.0,
		0.0, 0.0, 1.0, 1.0,
		0.0, 0.0, 0.0, 1.0,
	};

	GLint attrib_color = glGetAttribLocation(program, "a_color");
	GLint attrib_colorU = glGetUniformLocation(program, "u_color");

	glEnableVertexAttribArray(attrib_color);
	glVertexAttribPointer(attrib_color, 4, GL_FLOAT, false, sizeof(float) * 4, triangle);
	glUniform4f(attrib_colorU, 1.0f, 0.0f, 1.0f, 1.0f);*/

	glBindVertexArray(VAO);
	// Dessiner l'élément array buffer
	glDrawElements(GL_TRIANGLES, ob.elements.size() , GL_UNSIGNED_SHORT, (void*)0);
	glBindVertexArray(0);

	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, g_FBO.FBO);
	GLuint width = glutGet(GLUT_WINDOW_WIDTH);
	GLuint height = glutGet(GLUT_WINDOW_HEIGHT);
	glBlitFramebuffer(0, 0, 0 + width, 0 + height, 0, 0, 0 + width, 0 + height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	glutSwapBuffers();
}

int main(int argc, char* argv[])
{
	string fileName;
	cout << "Entrez le nom du fichier OBJ a ouvrir" << endl;
	cin >> fileName;
	ob.Initialize(fileName, "  ");
	ob.getObjFaces(fileName, "  ", "/");

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	glutCreateWindow("ObjLoader");

	// Activation de la profondeur et des lights
	glEnable(GL_DEPTH_TEST);
	/*glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	GLfloat posLight0[] = { 0.0f,0.0f,-1.0f,0.0f };
	GLfloat lightColor[] = { 1.0f,1.0f,1.0f,1.0f };

	// Application de la position et de la couleur
	glLightfv(GL_LIGHT0, GL_POSITION, posLight0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);*/

	Initialize();

	glutReshapeFunc(Resize);
	glutIdleFunc(Update);
	glutDisplayFunc(Render);
	glutMainLoop();

	Terminate();

	return 0;
}