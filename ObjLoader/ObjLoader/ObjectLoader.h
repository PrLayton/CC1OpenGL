#pragma once
using namespace std;
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
class ObjectLoader
{
	private:
		vector<float> vertices;

	public:
		ObjectLoader();
		~ObjectLoader();

		vector<float> getVertices();

		vector<float> loadVertices(string file);

		//Charger un tableau contenant tous les �l�ments pr�c�d�s du pr�fixe voulu
		//Exemple : v pour certices
		vector<float> loadElements(string file, string prefix);
};

