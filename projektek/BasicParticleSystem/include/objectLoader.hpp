#pragma once
#include <gl/glew.h>
#include <gl/freeglut.h>

#include <iostream>
#include <string>
#include <fstream>
#include "model.hpp"
#include <vector>
#include <map>
#include <glm/glm.hpp>


class FacePoint
{
public:
	/// initializes members
	FacePoint();
	/// does nothing
	~FacePoint();
	/// 0 returns the vertex, 1 returns the texture coordinates, 2 returns the vertex normal
	int& operator[](int index);
	/// returns the vertex
	int & getVertex();
	/// returns the texture coordinates
	int & getVertexTexture();
	/// returns the vertex normal
	int & getVertexNormal();

private:
	int m_vertex;           //0
	int m_vertexTexture;    //1
	int m_vertexNormal;     //2
};

class Face
{
public:
	/// initializes member
	Face();
	/// copy constructor
	Face(const Face& other);
	/// deletes member pointer
	~Face();
	/// returns the requested facepoint in the array (0, 1, 2)
	FacePoint& operator[](int index);
	/// deep copy
	Face& operator=(const Face& other);
	/// deep copy
	Face& operator=(Face& other);
	/// returns the data of the face in string form
	string printFace();
private:
	FacePoint * m_pFacePoint;
};

class BoundingBox
{
	friend class ObjectLoader;
public:
	/// initialize members
	BoundingBox();
	/// get the minimum values
	glm::vec3 getMinVertex() const;
	/// get the maximum values
	glm::vec3 getMaxVertex() const;
	/// max x - min x
	float getXdistance();
	/// max y - min y
	float getYdistance();
	/// max z - min z
	float getZdistance();

private:
	glm::vec3 m_min_vertex;
	glm::vec3 m_max_vertex;
};

class ObjectLoader
{
public:
	/// does nothing
	ObjectLoader();
	/// does nothing
	~ObjectLoader();
	/// opens file, counts and parses data, closes file
	void loadObjFile(std::string filename);
	/// returns the vertices vector
	std::vector<glm::vec3> getVertices() const;
	/// returns the vertex normals vector
	std::vector<glm::vec3> getVertexNormals() const;
	/// returns the vertex texture coordinates vector
	std::vector<glm::vec2> getTextureCoords() const;
	/// returns the map of vectors of faces, and the corresponding texture filenames
	std::map<std::string, std::vector<Face>> getFaceLists() const;
	/// returns the bounding box (2 vertices)
	BoundingBox& getBoundingBox();

private:
	unsigned int m_num_vertices = 0;
	unsigned int m_num_vertexTextures = 0;
	unsigned int m_num_vertexNormals = 0;
	unsigned int m_num_materials = 0;
	void countTokens();

	unsigned int m_current_faceList = 0;
	std::string getTextureFileName(const std::string& textureAlias) const;
	int getFaceListIndex(const std::string& textureAlias) const;

	std::vector<glm::vec3> m_vertices;
	std::vector<glm::vec3> m_vertexNormals;
	std::vector<glm::vec2> m_textureCoords;
	std::map<std::string, std::vector<Face>> m_faceLists;

	/// the lengths of the facelists in order
	int * m_faceList_lengths;
	void readData();

	/// read the corresponding .mtl, extracts texture aliases and filenames, 
	/// initializes m_faceLists and m_faceList_length
	void readMtlFile(string filename);
	/// string pair: Txtr Alias + Txtr Filename
	/// the first txtr belongs to the first faceList etc...
	std::vector<std::pair<std::string, std::string>> m_texture_aliases;

	BoundingBox m_boundingBox;
	/// checks if the vertex has smaller or greater values than the bounding box, and corrects the BB
	void compareBoundingBox(glm::vec3& vertex);

	ifstream m_file;
};
