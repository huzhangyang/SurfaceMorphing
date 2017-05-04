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
		//cout << "File " << filename << " Read Complete." << endl;
	}
	else
	{
		cout << "File " << filename << "Open Failed." << endl;
	}
	return mesh;
}

vector<string> MeshLoader::ParseFilename(string input)
{
	vector<string> ret;
	auto index = input.find(':');
	if (index != string::npos)
	{// pattern 2
		int startindex = stoi(input.substr(0, index));
		int endindex = stoi(input.substr(index + 1, input.size() - index));
		for (int i = startindex; i <= endindex; i++)
		{
			ret.push_back(to_string(i).append(".obj"));
		}
	}
	else
	{// pattern 1
		ret = split(input, ",");
		for (int i = 0; i < ret.size(); i++)
		{
			ret[i].append(".obj");
		}
	}

	return ret;
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