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

//Structure remplis par la première lecture du obj
std::vector<float> positions = {};
std::vector<float> texcoords = {};
std::vector<float> normales = {};

// Tableau final avec touts les élements de l'obj, sauf la forme: v1, v2, v3, vt1, vt2, vn1, vn2, vn3, v1...
// On aura un seul VBO
std::vector<float> points = { 1.000000f, -1.000000f, -1.000000f, 0.748573f, 0.750412f,
1.000000f, -1.000000f, 1.000000f, 0.749279f, 0.501284f,
-1.000000f, -1.000000f, 1.000000f, 1.0f, 0.0f,
-1.000000f, -1.000000f, -1.000000f, 1.0f, 1.0f,
1.000000f, 1.000000f, -1.000000f, 0.749279f, 0.501284f,
0.999999f, 1.000000f, 1.000001f, 0.0f, 1.0f,
-1.000000f, 1.000000f, -1.000000f, 1.0f, 0.0f };
// Tableau final des indices qui se remplie au fil de l'algo avec des indices de sommets de "points" 
std::vector<GLushort> indices = { 5, 1, 4,
								5, 4, 8,
								3, 7, 8,
								3, 8, 4,
								2, 6, 3,
								6, 7, 3,
								1,5,2,
								5,6,2};
// Liste des sommets déja lut (combinaisont de v/vt/vn)
std::vector<GLuint> listOfReadedPoint;

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
bool Framebuffer::CreateFBO(GLuint w, GLuint h)
{
	//ColorTex0 = CreateTexture(w, h);
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

	GLuint textureObj;
	//Demander de l'identifiant
	glGenTextures(1, &textureObj);
	glBindTexture(GL_TEXTURE_2D, textureObj);
	// Allocation
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	// Paramètres de la texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

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
	glewInit();
	basic.LoadVertexShader("shader.vs");
	basic.LoadFragmentShader("shader.fs");
	basic.Create();

	// On récupère l'identifiant de la texture créée
	textureID = CreateTexture("Koala.jpg");

	//pointArray
	/*const float carre[] = {
		-0.5f, 0.5f, 0.0f, 0.0f,	
		-0.5f, -0.5f, 0.0f, 1.0f,	
		0.5f, 0.5f, 1.0f, 0.0f,		
		0.5f, -0.5f, 1.0f, 1.0f		
	};*/

	// Index de sommet que l'on peut créer
	int index = 0;
	// Pour chaque f (face)
	for (GLushort i = 0; i < 12; i++)
	{
		// Pour chaque trio de tuple (v/vt/vn)
		for (GLushort j = 0; j < 3; j++)
		{
			// On par du principe que le point n'existe pas
			bool vertextExist = false;
			// On lit ce qu'il y a dans le fichier (ici la ligne 36 de objet2)
			int nbReadForV = 5;
			int nbReadForVT = 1;
			int nbReadForVN = 1;
			GLushort k;
			// On parcourt la liste des points déja lu et on regarde si la combinaison v/vt/vn à déja été traitée
			for ( k = 0; k < listOfReadedPoint.size()-3; k+=3)
			{
				if (nbReadForV == listOfReadedPoint[k] && nbReadForVT == listOfReadedPoint[k + 1] && nbReadForVN == listOfReadedPoint[k + 2]) {
					vertextExist = true;
					break;
				}
			}
			// Si le point n'éxiste pas
			if (!vertextExist) {
				// On push les coordonées du sommets
				points.push_back(positions[(nbReadForV - 1) * 3]);
				points.push_back(positions[(nbReadForV - 1) * 3 + 1]);
				points.push_back(positions[(nbReadForV - 1) * 3 + 2]);

				// Les coordonées de texture
				points.push_back(texcoords[(nbReadForVT - 1) * 2]);
				points.push_back(texcoords[(nbReadForVT - 1) * 2 + 1]);

				// Les normales
				points.push_back(normales[(nbReadForVN - 1) * 3]);
				points.push_back(normales[(nbReadForVN - 1) * 3 + 1]);
				points.push_back(normales[(nbReadForVN - 1) * 3 + 2]);

				// Le point unique à été lu, on l'ajoute dans la liste des points déja existants
				listOfReadedPoint.push_back(nbReadForV);
				listOfReadedPoint.push_back(nbReadForVT);
				listOfReadedPoint.push_back(nbReadForVN);

				// On crée un nouvel indice (le sommet n'existait pas)
				indices.push_back(index++);
			}
			else
			{
				// On récupère l'indice du point que l'on a déja lu
				indices.push_back(k/3);
			}
		}
	}

	//const GLushort indices[] = { 0, 1, 2, 3 };

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
	glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, false, sizeof(float) * 4, 0);
	glEnableVertexAttribArray(positionAttrib);
	auto texcoordsAttrib = glGetAttribLocation(program, "a_texcoords");
	glVertexAttribPointer(texcoordsAttrib, 2, GL_FLOAT, false, sizeof(float) * 4, (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(texcoordsAttrib);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	// Ne rend plus actif
	// Permet de desactiver le VAO
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// Désactive ce dont on ne se sert plus
	glEnableVertexAttribArray(0);
}

void Terminate()
{
	g_FBO.DestroyFBO();
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
	// Pour la partie 2
	glBindFramebuffer(GL_FRAMEBUFFER, g_FBO.FBO);

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);

	auto program = basic.GetProgram();
	glUseProgram(program);

	// Matrices
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

	//Rendre actif sur l'unité de texture 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);
	auto textureLoc0 = glGetUniformLocation(program, "u_texture0");
	glUniform1i(textureLoc0, 0);

	glBindVertexArray(VAO);
	// Dessiner l'élément array buffer
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

	// Activation de la profondeur et des lights
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	GLfloat posLight0[] = { 0.0f,0.0f,-1.0f,0.0f };
	GLfloat lightColor[] = { 1.0f,1.0f,1.0f,1.0f };

	// Application de la position et de la couleur
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