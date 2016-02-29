#include "ObjectLoader.h"



ObjectLoader::ObjectLoader()
{
}


ObjectLoader::~ObjectLoader()
{
	vertices.clear();
}

vector<float> ObjectLoader::getVertices()
{
	return vertices;
}

vector<float> ObjectLoader::loadVertices(string file)
{
	vector<float> v;
	string line = "line";
	string delimiter = " ";
	string temp = "temp";
	size_t pos(0);

	//Faire une vérification pour avoir obligatoirement un OBJ
	cout << "Chargement du fichier OBJ : " << file << endl;

	ifstream myfile (file, ios::in);

	vector<float> tempC;

	tempC.push_back(48);
	tempC.push_back(52);
	tempC.push_back(36);

	int i = 0;

	if (myfile.is_open())
	{
		while (getline(myfile, line))
		{
			if (line.substr(0, 1).compare("v") == 0) {
				//On supprime le V
				temp = line.substr(0, pos);
				std::cout << temp << std::endl;
				line.erase(0, pos + delimiter.length()+1);

				while ((pos = line.find(delimiter)) != std::string::npos) {
					
					//On prend X et Y
					temp = line.substr(0, pos);
					//if(temp.substr(0, 1) == "-")
					std::cout << temp << " ";
					line.erase(0, pos + delimiter.length());
					tempC[i] = stof(temp);
					
					i++;
				}

				//On prend Z
				std::cout << line << " " << endl;
				tempC[i] = stof(line);
				i = 0;
				pos = 0;
				v.push_back(tempC[0]);
				v.push_back(tempC[1]);
				v.push_back(tempC[2]);
			}
		}

		myfile.close();
	}
	else
	{
		cout << "Impossible de charger le fichier" << file << endl;
	}
	

	cout << "Le fichier" << file << "a été chargé" << endl;

	return v;
}

vector<float> ObjectLoader::loadElements(string file, string prefix)
{
	vector<float> v;
	string line = "line";
	string delimiter = " ";
	string temp = "temp";
	size_t pos(0);

	//Faire une vérification pour avoir obligatoirement un OBJ
	cout << "Chargement du fichier OBJ : " << file << endl;

	ifstream myfile(file, ios::in);

	vector<float> tempC;

	tempC.push_back(48);
	tempC.push_back(52);
	tempC.push_back(36);

	int i = 0;

	if (myfile.is_open())
	{
		while (getline(myfile, line))
		{
			if (line.substr(0, 1).compare(prefix) == 0) {
				//On supprime le V
				temp = line.substr(0, pos);
				std::cout << temp << std::endl;
				line.erase(0, pos + delimiter.length() + 1);

				while ((pos = line.find(delimiter)) != std::string::npos) {

					//On prend X et Y, etc...
					temp = line.substr(0, pos);
					//if(temp.substr(0, 1) == "-")
					std::cout << temp << " ";
					line.erase(0, pos + delimiter.length());
					v.push_back(stof(temp));

					i++;
				}
				//On prend la dernière composante
				std::cout << line << " " << endl;
				v.push_back(stof(line));
				i = 0;
				pos = 0;

			}
		}

		myfile.close();
	}
	else
	{
		cout << "Impossible de charger le fichier" << file << endl;
	}


	cout << "Le fichier" << file << "a été chargé" << endl;

	return v;
}
