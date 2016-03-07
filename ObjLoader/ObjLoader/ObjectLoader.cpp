#include "ObjectLoader.h"



ObjectLoader::ObjectLoader()
{
}


ObjectLoader::~ObjectLoader()
{
	vertices.clear();
}

void ObjectLoader::Initialize(string file, string delimiter_)
{
	vertices = loadElements(file, "v", delimiter_);
	tex = loadElements(file, "vt", delimiter_);
	normals = loadElements(file, "vn", delimiter_);
}

vector<float> ObjectLoader::getVertices()
{
	return vertices;
}

vector<float> ObjectLoader::loadVertices(string file)
{
	vector<float> v;
	vertices = vector<float>();
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
				vertices.push_back(tempC[0]);
				vertices.push_back(tempC[1]);
				vertices.push_back(tempC[2]);

			}
		}

		myfile.close();
	}
	else
	{
		cout << "Impossible de charger le fichier" << file << endl;
	}
	

	cout << "Le fichier" << file << "a été chargé" << endl;

	return vertices;
}

vector<float> ObjectLoader::loadElements(string file, string prefix, string delimiter_)
{
	vector<float> v;
	string line = "line";
	string delimiter = delimiter_;
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
			if (line.substr(0, line.find(" ")).compare(prefix) == 0) {
				//On supprime le V
				temp = line.substr(0, pos);
				line.erase(0, pos + delimiter.length() + 1);

				for (int i = 0; i < line.length(); i++) {
					//start
					if (line[i] == ' ' || line[i] == '\n') {
						if (temp.length() > 0) {
							if(temp.compare(" ") != 0)
							v.push_back(stof(temp));
						}
						temp = "";
					}

					temp += line[i];

					if (i == line.length() - 1) {
						if (temp.compare(" ") != 0)
							v.push_back(stof(temp));
					}

				}

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

vector<float> ObjectLoader::getObjFaces(string file, string tupleDelimiter, string tripletDelimiter)
{
	indices.clear();
	indices = vector<int>();

	elements.clear();
	elements = vector<float>();
	vector<int> tempIndices;
	string line = "line";
	string temp = "temp";
	size_t pos(0);

	int cNumber(3); //Nombre de constantes faisant partie des triplets a/b/c

	vector<string> indexedElements;
	string indexedLine;
	bool canAdd = true;

	//Faire une vérification pour avoir obligatoirement un OBJ
	cout << "Chargement des faces..." << endl;

	ifstream myfile(file, ios::in);

	if (myfile.is_open())
	{

		while (getline(myfile, line))
		{
			if (line.substr(0, 1).compare("f") == 0) {
				
				size_t tripletPos(0);
				//On supprime le f
				temp = line.substr(0, pos);
				pos = line.find(tupleDelimiter);
				line.erase(0, pos + tupleDelimiter.length() );
				temp = "";
				for (int i = 0; i < line.length(); i++) {
					//start
					char a = line[i];
					if (line[i] == ' ' || line[i] == '/') {
						if (temp.length() > 0) {
							if (temp.compare(" ") != 0 && temp.compare("/") != 0) {
								tempIndices.push_back(stoi(temp));
								cout << temp << " ";
							}
								
						}
						if (i < line.length() - 1 && line[i] == '/' && line[i + 1] == '/') {
							tempIndices.push_back(1);
						}
						temp = "";
					}
					else {
						temp += line[i];

						if (i == line.length() - 1) {
							if (temp.compare(" ") != 0) {
								tempIndices.push_back(stoi(temp));
								cout << temp << " ";
							}
								
							
						}
					}

					

				}
				/*
				//On trouve les triplets A/B/C en les isolants dans des tableaux
				while ((pos = line.find(tupleDelimiter)) != string::npos)
				{
					temp = line.substr(0, pos);

					//On sépare chacune des parties du triplet
					
					cNumber = 1;
					while ((tripletPos = temp.find(tripletDelimiter)) != std::string::npos) { //On vérifie si il reste encore le tuplDelimiter
						tempIndices.push_back(stoi(temp.substr(0, tripletPos)));
						temp.erase(0, tripletPos + tripletDelimiter.length());
						cNumber++;
					}
					tempIndices.push_back(stoi(temp));

					//On supprime le triplet pour passer au suivant
					line.erase(0, pos + tupleDelimiter.length());

					cout << temp << endl;

					
				}
				temp = line.substr(0, pos);
				while ((tripletPos = temp.find(tripletDelimiter)) != std::string::npos) { //On vérifie si il reste encore le tuplDelimiter
					tempIndices.push_back(stoi(temp.substr(0, tripletPos)));
					temp.erase(0, tripletPos + tripletDelimiter.length());
				}
				tempIndices.push_back(stoi(temp));
				*/
			}
			cout << endl;
		}

		myfile.close();
	}
	else
	{
		cout << "Impossible de charger le fichier" << file << endl;
	}

	//Une fois qu'on a récupéré toutes les informations du fichier, on va les trier pour avoir toutes les valeurs dans le bon ordre
	/*
	for (int a = 0; a < cNumber; a++) {
		for (int i = a; i < tempC.size(); i += cNumber) {
			v.push_back(tempC[i]);
		}
	}
	*/
	/*
	int indice(0);
	for (int i = 0; i < tempIndices.size(); i+= cNumber*3) {
		for (int a = 0; a < cNumber; a++) {
			for (int j = 0; j < cNumber * 3; j += cNumber) {
				indice = tempIndices[j + i + a];
				indices.push_back(indice);
				
			}
		}
		
	}
	*/

	if (tex.size() < 1) {
		tex.assign(vertices.size(), 0);
	}

	if (normals.size() < 1) {
		normals.assign(vertices.size(), 0);
	}


	
	int precIndex = 0;
	for (int i = 0; i < tempIndices.size(); i += 3) {

		indexedLine = (to_string(tempIndices[i]) + to_string(tempIndices[i+1]) + to_string(tempIndices[i+2]));

		canAdd = true;
		for (int b = 0; b < indexedElements.size(); b++) {

			//On compare les différents triplets pour savoir si ils sont égaux et donc ne pas les ajouter dans le tableau
			precIndex = b;
			if (indexedElements[b].compare(indexedLine) == 0 ) {
				canAdd = false;
				
				break;
			}
			

		}

		if (canAdd) {
			indexedElements.push_back(indexedLine);

			for (int j = 0; j < 3; j++) {
				elements.push_back(vertices[tempIndices[i] - 1 + j]);
			}
			for (int j = 0; j < 2; j++) {
				elements.push_back(tex[tempIndices[i + 1] - 1 + j]);
			}
			for (int j = 0; j < 3; j++) {
				elements.push_back(normals[tempIndices[i + 2] - 1 + j]);
			}

			//On ajoute un autre indice car on a trouvé aucun point correspondant
			
		}
		indices.push_back(precIndex);
	}
	cout << "Le fichier" << file << "a été chargé" << endl;

	return elements;
}
