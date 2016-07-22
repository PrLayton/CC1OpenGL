#ifdef _WIN32

// Librairies
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

// Fonction de travail
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
// Pour rajouter un z buffer
GLuint RBO;

// L'objet qui va travailler avec l'obj
ObjectLoader ob = ObjectLoader();

// Tableau final avec touts les élements de l'obj, sous la forme: v1, v2, v3, vt1, vt2, vn1, vn2, vn3, v1...
std::vector<float> points = {};
// Tableau final des indices qui se remplie au fil de l'algo avec des indices de sommets de "points" 
std::vector<GLushort> indices = {};

// Buffer Structure
struct Framebuffer
{
	GLuint ColorTex0;
	GLuint FBO;

	bool CreateFBO(GLuint w, GLuint h);
	void DestroyFBO();
};
Framebuffer _frameBuffer;

// FBO fonctions
GLuint CreateEmptyTexture(GLuint w, GLuint h)
{
	GLuint textureObj;
	glGenTextures(1, &textureObj);
	glBindTexture(GL_TEXTURE_2D, textureObj);
	//Autre méthode pour avoir la version noir et blanc
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_INTENSITY, w, h, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, nullptr);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	return textureObj;
}
bool Framebuffer::CreateFBO(GLuint w, GLuint h)
{
	ColorTex0 = CreateEmptyTexture(w, h);
	glGenFramebuffers(1, &FBO);

	/*// Z Buffer
	glGenRenderbuffers(1, &RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, w, h);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);*/

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

// Fonctions pour la tex
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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	// Paramétres de la texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glHint(GL_GENERATE_MIPMAP_HINT, GL_FASTEST);
	GLfloat maxAniso;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAniso);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAniso);
	glGenerateMipmap(GL_TEXTURE_2D);


	stbi_image_free(image);

	//Dessiner sans texture pour le moment
	glBindTexture(GL_TEXTURE_2D, 0);

	return textureObj;
}
void DestroyTexture(GLuint textureObj)
{
	glDeleteTextures(1, &textureObj);
}

// Initialisation des données
void Initialize()
{
	// Récupération des éléments générés
	points = ob.elements;
	indices = ob.getIndicesToGLushort();

	// Chargement du shader
	glewInit();
	basic.LoadVertexShader("shader.vs");
	basic.LoadFragmentShader("shader.fs");
	basic.Create();

	// On récupére l'identifiant de la texture créée
	textureID = CreateTexture("wood.jpg"); //Koala.jpg

	// Création du vertex buffer object
	glGenBuffers(1, &VBO);
	// Définiti comme actif
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// Allocation
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * points.size(), &points[0], GL_STATIC_DRAW);

	// Création de l'index buffer object
	glGenBuffers(1, &IBO);
	// Définiti comme actif
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	// Allocation
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * indices.size(), &indices[0], GL_STATIC_DRAW);

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
	//On passe les vertices
	glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, false, sizeof(float) * ob.indiceLength, 0);
	glEnableVertexAttribArray(positionAttrib);

	// Si on a des coordonnées de texture
	if (ob.hasTexture) {
		auto texcoordsAttrib = glGetAttribLocation(program, "a_texcoords");
		//On passe les coordonnées de texture
		glVertexAttribPointer(texcoordsAttrib, 2, GL_FLOAT, false, sizeof(float) * ob.indiceLength, (void*)(((ob.hasNormals) ? ob.indiceLength - 5 : ob.indiceLength -2) * sizeof(float)));
		glEnableVertexAttribArray(texcoordsAttrib);
	}

	// Si on a des coordonnées pour les normales
	if (ob.hasNormals) {
		// Fait correspondre le shader et les normales
		auto normalAttrib = glGetAttribLocation(program, "a_normal");
		//On passe les coordonnées de normales
		glVertexAttribPointer(normalAttrib, 3, GL_FLOAT, false, sizeof(float) * ob.indiceLength, (void*)(ob.indiceLength - 3 * sizeof(float)));
		glEnableVertexAttribArray(normalAttrib);
	}


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	// Ne rend plus actif
	// Permet de desactiver le VAO
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	//glEnableVertexAttribArray(0);

	bool valid = _frameBuffer.CreateFBO(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	//bool valid = FBO.CreateFBO(texW, texH);
	assert(valid == true);
}

// Destruction des objects à la fin du programme
void Terminate()
{
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &IBO);

	_frameBuffer.DestroyFBO();

	DestroyTexture(textureID);
	basic.Destroy();
}

// Pour le redimesionnement de la fenêtre
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
	// Première passe
	glBindFramebuffer(GL_FRAMEBUFFER, _frameBuffer.FBO);

	// Passe de couleur de fond
	glClearColor(0.8f, 0.7f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	// Active le masquage de faces
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	auto program = basic.GetProgram();
	glUseProgram(program);

	auto postProcess = glGetUniformLocation(program, "u_postProcess");
	glUniform1i(postProcess, false);

	// Matrices
	const float scaleMatrix[]{
		0.4f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.4f,0.0f, 0.0f,
		0.0f, 0.0f, 0.4f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
	auto scaleLocation = glGetUniformLocation(program, "u_scaleMatrix");
	glUniformMatrix4fv(scaleLocation, 1, GL_FALSE, scaleMatrix);

	// Rotation autour de z
	float time = glutGet(GLUT_ELAPSED_TIME) / 3000.0f;
	const float rotationMatrix[]{
		cos(time), sin(time), 0.0f, 0.0f,
		-sin(time), cos(time),0.0f, 0.0f,
		//0.0f, cos(time), -sin(time), 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
	auto rotationLocation = glGetUniformLocation(program, "u_rotationMatrix");
	glUniformMatrix4fv(rotationLocation, 1, GL_FALSE, rotationMatrix);

	// Rotation autour de y
	const float rotationMatrix2[]
	{ cos(time), 0.0f, sin(time), 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		-sin(time), 0.0f, cos(time), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f };
	auto rotationLocation2 = glGetUniformLocation(program, "u_rotationMatrix2");
	glUniformMatrix4fv(rotationLocation2, 1, GL_FALSE, rotationMatrix2);

	float right = glutGet(GLUT_WINDOW_WIDTH) / 2.0f;
	float left = -right;
	float top = glutGet(GLUT_WINDOW_HEIGHT) / 2.0f;
	float bottom = -top;
	float _near = -1.f;
	float _far = 1.f;
	// Projection orthographic
	/*const float projectionMatrix[]{
	2.0f / (right - left), 0.0f, 0.0f, 0.0f,
	0.0f, 2.0f / (top - bottom),0.0f, 0.0f,
	0.0f, 0.0f, - 2.0f / (_far - _near), 0.0f,
	-(right+ left) / (right- left), -(top + bottom) / (top - bottom), -(_far + _near) / (_far - _near), 1.0f
	};*/
	// Projection perspective
	const float projectionMatrix[]{
		2.0f * _near / (right - left), 0.0f, 0.0f, 0.0f,
		0.0f, 2.0f * _near / (top - bottom),0.0f, 0.0f,
		(right + left) / (right - left), (top + bottom) / (top - bottom), -(_far + _near) / (_far - _near) , - 1.0f,
		0.0f, 0.0f, - 2.0f * _far * _near / (_far - _near), 0.0f
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

	glBindVertexArray(VAO);
	// Dessiner l'élément array buffer
	glDrawElements(GL_TRIANGLES, ob.elements.size(), GL_UNSIGNED_SHORT, (void*)0);
	glBindVertexArray(0);

	// On passe sur l'ecran
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	// Efface le buffer
	glClearColor(0.7f, 0.6f, 0.4f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);

	// Seconde passe
	glUniform1i(postProcess, true);

	glBindVertexArray(VAO);
	// Dessiner l'élément array buffer
	glDrawElements(GL_TRIANGLES, ob.elements.size(), GL_UNSIGNED_SHORT, (void*)0);
	glBindVertexArray(0);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, _frameBuffer.FBO);
	GLuint width = glutGet(GLUT_WINDOW_WIDTH);
	GLuint height = glutGet(GLUT_WINDOW_HEIGHT);
	glBlitFramebuffer(0, 0, width, height, width * 2 / 3, height * 2 / 3, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	glutSwapBuffers();
}

int main(int argc, char* argv[])
{
	string fileName;
	// Texte de description
	cout << "Bienvenue sur notre programme de chargement de fichier OBJ." << endl;
	cout << "Ce programme a ete realise par Julien Geiger et Baptiste Marechaux." << endl << endl;
	cout << "Entrez le nom du fichier OBJ a ouvrir (exemple: table.obj)" << endl;
	cout << "cube1 : pas de coordonnees de texture | cube2 : avec des coordonnees de texture" << endl;
	cin >> fileName;
	ob.Initialize(fileName, "  ");
	// Affichage des faces traitées
	ob.getObjFaces(fileName, "  ", "/");

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