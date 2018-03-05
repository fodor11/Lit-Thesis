#include "../include/objectLoader.hpp"

//read .mtl
void ObjectLoader::readMtlFile(std::string filename)
{
	cout << "Parsing .mtl...\t";
	ifstream mtlFile;
	mtlFile.open(filename);
	m_num_materials = 0;
	std::string line;
	//counting
	if (mtlFile.is_open())
	{
		while (getline(mtlFile, line))
		{
			if (line.compare(0, 7, "newmtl ") == 0)
			{
				m_num_materials++;
			}
		}
	}
	m_texture_aliases.resize(m_num_materials);

	//parsing
	mtlFile.clear();
	mtlFile.seekg(0, ios::beg);
	m_num_materials = 0;

	if (mtlFile.is_open())
	{
		while (getline(mtlFile, line))
		{
			if (line.compare(0, 7, "newmtl ") == 0)
			{
				m_texture_aliases[m_num_materials].first = line.substr(7);
			}
			else if (line.compare(0, 7, "map_Kd ") == 0)
			{
				m_texture_aliases[m_num_materials].second = line.substr(7);
				m_num_materials++;
			}
		}
	}
	
	mtlFile.close();
	m_faceList_lengths = new int[m_num_materials];
	cout << " Done." << endl;
}

int ObjectLoader::getFaceListIndex(const std::string& textureAlias) const
{
	for (int i = 0; i < m_num_materials; i++)
	{
		if (m_texture_aliases[i].first.compare(textureAlias) == 0)
		{
			return i;
		}
	}
}
std::string ObjectLoader::getTextureFileName(const std::string& textureAlias) const
{
	for (int i = 0; i < m_num_materials; i++)
	{
		if (m_texture_aliases[i].first.compare(textureAlias) == 0)
		{
			return m_texture_aliases[i].second;
		}
	}
}


void ObjectLoader::countTokens()
{
	m_num_vertices = 0;
	m_num_vertexTextures = 0;
	m_num_vertexNormals = 0;
	m_current_faceList = 0;
	
	string line;
	if (m_file.is_open())
	{
		cout << "Counting tokens in .obj file" << endl;
		while (getline(m_file, line))
		{
			if (line.compare(0, 7, "mtllib ") == 0)
			{
				readMtlFile(line.substr(7));
				for (int i = 0; i < m_num_materials; i++)
				{
					m_faceList_lengths[i] = 0;
				}
			}
			else if (line.compare(0,2,"v ")==0)
			{
				m_num_vertices++;
			}
			else if (line.compare(0, 2, "vt") == 0)
			{
				m_num_vertexTextures++;
			}
			else if (line.compare(0, 2, "vn") == 0)
			{
				m_num_vertexNormals++;
			}
			else if (line.compare(0, 7, "usemtl ") == 0)
			{
				m_current_faceList = getFaceListIndex(line.substr(7));
			}
			else if (line.compare(0, 2, "f ") == 0)
			{
				m_faceList_lengths[m_current_faceList]++;
			}
		}
	}
	cout << "v: " << m_num_vertices << "   vt: " << m_num_vertexTextures << "   vn: " << m_num_vertexNormals << endl;
	for (int i = 0; i < m_num_materials; i++)
	{
		cout << "facelist[" << i << "]: " << m_faceList_lengths[i] << endl;
	}

	m_vertices.reserve(m_num_vertices);
	m_vertexNormals.reserve(m_num_vertexNormals);
	m_textureCoords.reserve(m_num_vertexTextures);
	for (int i = 0; i < m_num_materials; i++)
	{
		m_faceLists[m_texture_aliases[i].second].reserve(m_faceList_lengths[i]);
	}
}

void parse3floats(glm::vec3& vector, std::string line)
{
	string::size_type nextFloat = 2; //jumping the vn/v
	for (int i = 0; i < 3; i++)
	{
		line = line.substr(nextFloat);
		vector[i] = stof(line, &nextFloat);
	}
}
void parse2floats(float* vector, std::string line)
{
	string::size_type nextFloat = 2; //jumping the vt
	for (int i = 0; i < 2; i++)
	{
		line = line.substr(nextFloat);
		vector[i] = stof(line, &nextFloat);
	}
}
void parseFace(Face& face, std::string line)
{
	string::size_type nextInt = 1;
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			line = line.substr(nextInt+1);	//+1 for jumping '/' or ' '
			face[i][j] = stoi(line, &nextInt);
		}
	}
}
void ObjectLoader::compareBoundingBox(glm::vec3& vertex)
{
	for (int i = 0; i < 3; i++)
	{
		if (vertex[i] < m_boundingBox.m_min_vertex[i])
		{
			m_boundingBox.m_min_vertex[i] = vertex[i];
		}
		else if (vertex[i] > m_boundingBox.m_max_vertex[i])
		{
			m_boundingBox.m_max_vertex[i] = vertex[i];
		}
	}
}
void ObjectLoader::readData()
{
	//clear EOF flag and return to the beginning
	m_file.clear();
	m_file.seekg(0, ios::beg);
	Face tmpFace;
	glm::vec3 tmpVector;
	float tmpFloatArray[2];
	
	std::string current_faceList;

	std::string line;
	if (m_file.is_open())
	{
		cout << "Parsing Data" << endl;
		while (getline(m_file, line))
		{
			if (line.compare(0, 2, "v ") == 0)
			{
				parse3floats(tmpVector, line);
				m_vertices.push_back(tmpVector);
				compareBoundingBox(m_vertices.back());
			}
			else if (line.compare(0, 2, "vt") == 0)
			{
				parse2floats(tmpFloatArray, line);
				m_textureCoords.push_back(glm::vec2(tmpFloatArray[0], tmpFloatArray[1]));
			}
			else if (line.compare(0, 2, "vn") == 0)
			{
				parse3floats(tmpVector, line);
				m_vertexNormals.push_back(tmpVector);	
			}
			else if (line.compare(0, 7, "usemtl ") == 0)
			{
				current_faceList = getTextureFileName(line.substr(7));
			}
			else if (line.compare(0, 2, "f ") == 0)
			{
				parseFace(tmpFace, line);
				m_faceLists[current_faceList].push_back(tmpFace);
			}
		}
	}
	std::cout << "Parsing done" << endl;
}

ObjectLoader::ObjectLoader(){}

ObjectLoader::~ObjectLoader(){}

void ObjectLoader::loadObjFile(std::string filename)
{
	m_file.open(filename);
	//do stuff
	countTokens();
	readData();

	m_file.close();
}

std::vector<glm::vec3>  ObjectLoader::getVertices() const
{
	return m_vertices;
}

std::vector<glm::vec3>  ObjectLoader::getVertexNormals() const
{
	return m_vertexNormals;
}

std::vector<glm::vec2> ObjectLoader::getTextureCoords() const
{
	return m_textureCoords;
}

std::map<std::string, std::vector<Face>> ObjectLoader::getFaceLists() const
{
	return m_faceLists;
}

BoundingBox & ObjectLoader::getBoundingBox()
{
	return m_boundingBox;
}

FacePoint::FacePoint()
{
	m_vertex = 0;
	m_vertexTexture = 0;
	m_vertexNormal = 0;
}

FacePoint::~FacePoint()
{
}

int & FacePoint::operator[](int index)
{
	switch (index)
	{
	case 0:
		return m_vertex;
		break;
	case 1:
		return m_vertexTexture;
		break;
	case 2:
		return m_vertexNormal;
		break;
	}
}

int & FacePoint::getVertex()  
{
	return m_vertex;
}

int & FacePoint::getVertexTexture() 
{
	return m_vertexTexture;
}

int & FacePoint::getVertexNormal() 
{
	return m_vertexNormal;
}

Face::Face()
{
	m_pFacePoint = new FacePoint[3];
}

Face::Face(const Face & other)
{
	m_pFacePoint = new FacePoint[3];
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			m_pFacePoint[i][j] = other.m_pFacePoint[i][j];
		}
	}
}

Face::~Face()
{
	delete[] m_pFacePoint;
}

FacePoint & Face::operator[](int index)
{
	if (index>=0 && index <=2)
	{
		return m_pFacePoint[index];
	}
}

Face & Face::operator=(const Face & other)
{
	delete[] m_pFacePoint;
	m_pFacePoint = new FacePoint[3];
	for (int i = 0; i < 3; i++)
	{	
		for (int j = 0; j < 3; j++)
		{
			m_pFacePoint[i][j] = other.m_pFacePoint[i][j];
		}
	}
	return *this;
}
Face & Face::operator=(Face & other)
{
	delete[] m_pFacePoint;
	m_pFacePoint = new FacePoint[3];
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			m_pFacePoint[i][j] = other.m_pFacePoint[i][j];
		}
	}
	return *this;
}


string Face::printFace()
{
	string ret = "";
	for (int i = 0; i < 3; i++)
	{
		ret += std::to_string(m_pFacePoint[i][0]) + "/";
		ret += std::to_string(m_pFacePoint[i][1]) + "/";
		ret += std::to_string(m_pFacePoint[i][2]) + " ";
	}

	return ret;
}

BoundingBox::BoundingBox()
{
	m_min_vertex = glm::vec3();
	m_max_vertex = glm::vec3();
}

glm::vec3 BoundingBox::getMinVertex() const
{
	return m_min_vertex;
}

glm::vec3 BoundingBox::getMaxVertex() const
{
	return m_max_vertex;
}

float BoundingBox::getXdistance()
{
	return m_max_vertex[0] - m_min_vertex[0];
}

float BoundingBox::getYdistance()
{
	return m_max_vertex[1] - m_min_vertex[1];
}

float BoundingBox::getZdistance()
{
	return m_max_vertex[2] - m_min_vertex[2];
}
