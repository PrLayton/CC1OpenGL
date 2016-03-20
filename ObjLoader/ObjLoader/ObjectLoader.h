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

		//Charger un tableau contenant tous les éléments précédés du préfixe voulu
		//Exemple : v pour certices
		vector<float> loadElements(string file, string prefix, string delimiter_);

		vector<float> getObjFaces(string file, string tupleDelimiter /*Ce qui sépare les n*uplets de toute la ligne*/, string tripletDelimiter /*ce qui sépare les n éléments de chaque n*uplet, on considère temporairement qu'il y en a toujours 3*/);



};

