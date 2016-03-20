#pragma once
using namespace std;
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#include "GL/glew.h"
#include "GL/freeglut.h"
class ObjectLoader
{
	private:


	public:
		ObjectLoader();
		~ObjectLoader();

		bool hasNormals;
		bool hasTexture;

		int indiceLength = ;

		vector<float> vertices;
		vector<float> tex;
		vector<float> normals;

		vector<float> elements;
		vector<int> indices;

		void Initialize(string file, string delimiter_);

		vector<float> getVertices();

		vector<float> loadVertices(string file);

		vector<GLushort> getIndicesToGLushort();

		//Charger un tableau contenant tous les �l�ments pr�c�d�s du pr�fixe voulu
		//Exemple : v pour certices
		vector<float> loadElements(string file, string prefix, string delimiter_);

		vector<float> getObjFaces(string file, string tupleDelimiter /*Ce qui s�pare les n*uplets de toute la ligne*/, string tripletDelimiter /*ce qui s�pare les n �l�ments de chaque n*uplet, on consid�re temporairement qu'il y en a toujours 3*/);



};

