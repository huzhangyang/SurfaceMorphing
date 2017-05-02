#include "MeshLoader.h"

Mesh* MeshLoader::LoadObj2D(string filename)
{
	Mesh* mesh = new Mesh();
	ifstream file(filename);
	if (file.is_open())
	{
		while (!file.eof())
		{
			string header;
			file >> header;
			//cout << "Read from file: " << header << endl;

			if (header.compare("v") == 0)
			{
				vec3 vertex;
				file >> vertex.x >> vertex.y >> vertex.z;
				mesh->rawVertices.push_back(vertex);
			}
			else if (header.compare("f") == 0)
			{
				int v1, v2, v3;
				file >> v1 >> v2 >> v3;
				mesh->vertexIndices.push_back(v1 - 1);
				mesh->vertexIndices.push_back(v2 - 1);
				mesh->vertexIndices.push_back(v3 - 1);
			}
		}

		for (unsigned int i = 0; i< mesh->vertexIndices.size(); i++)
		{
			unsigned int vertexIndex = mesh->vertexIndices[i];
			vec3 vertex = mesh->rawVertices[vertexIndex];
			mesh->outVertices.push_back(vertex);
		}

		file.close();
		cout << "File Read Complete." << endl;
	}
	else
	{
		cout << "File Open Failed." << endl;
	}
	return mesh;
}

vector<string> MeshLoader::split(string& s, const char* delim)
{
	vector<string> ret;
	size_t last = 0;
	size_t index = s.find_first_of(delim, last);
	while (index != string::npos)
	{
		ret.push_back(s.substr(last, index - last));
		last = index + 1;
		index = s.find_first_of(delim, last);
	}
	if (index - last>0)
	{
		ret.push_back(s.substr(last, index - last));
	}

	return ret;
}