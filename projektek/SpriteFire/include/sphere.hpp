#pragma once
#include "../src/tdogl/Program.h"
#include <vector>
#include <glm/glm.hpp>

/// calculates the vertices, texture coordinates, normals and indicies (ccw) of a shpere for VBOs
class Sphere {
public:
	/// initializes members and creates the vectors
	Sphere(float radius, unsigned int longitude, unsigned int latitude, bool normalDirectsOutwards);
	/// does nothing
	~Sphere();
	/// returns the vector of vertices
	std::vector<glm::vec3>& getVertices();
	/// returns the vector of normals
	std::vector<glm::vec3>& getNormals();
	/// returns the vector of texture coordinates
	std::vector<glm::vec2>& getTextureCoordinates();
	/// returns the vector of texture coordinates
	std::vector<glm::ivec3>& getIndicies();
private:
	float m_fRadius;
	unsigned int m_iLongitude;
	unsigned int m_iLatitude;
	bool m_bNormalDirectsOutwards;

	std::vector<glm::vec3> m_vVertices;
	std::vector<glm::vec2> m_vTextureCoords;
	std::vector<glm::vec3> m_vNormals;
	std::vector<glm::ivec3> m_vIndices;

	void createVectors();
};