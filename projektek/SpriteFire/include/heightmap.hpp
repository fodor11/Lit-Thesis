#pragma once
#include <gl/glew.h>
#include <gl/freeglut.h>

#include <memory>
#include <soil/SOIL.h>
#include <string>
#include "model.hpp"

#include "../src/tdogl/Program.h"

typedef unsigned char Pixel[3];
class vec3f;

class HeightMapLoader
{
public:
	HeightMapLoader(std::string fileName, tdogl::Program* program);
	void drawTerrain() const;
	~HeightMapLoader();
	int getImageWidth() const;
	int getImageHeight() const;
	/// get the normalized height value [0,1]
	float getUnitHeight(float x, float z);
	/// returns the "true" height (unit height * max height)
	float getHeight(float x, float z);
	float getScale() const;
	float getMaxHeight() const;

private:
	tdogl::Program* m_pProgram;
	glm::mat4 m_mModel = glm::mat4();
	unsigned char* m_pImage;
	std::string m_fileName;
	int m_height;
	int m_width;
	int m_channels;

	GLuint m_iTextureID;
	GLuint m_iTerrainVAO;
	std::vector<GLuint> m_vVBOs;
	GLuint m_iTerrainVBO;
	GLuint m_iIndexVBO;
	GLuint m_iNumberOfIndicies;

	float m_highestPoint = 0.4f;
	float m_lowestPoint = 0.4f;
	float m_wettestPoint = 0.4f;
	float m_dryestPoint = 0.4f;

	float *m_pHeightValues;
	vec3f *m_pVertexNormals;
	float *m_pMoisture;

	vec3f *m_pColors;
	float *m_pShininess;

	float m_scale = 1.f;
	float m_maxHeight = 90 * m_scale;

	void getMoistValues();
	void getHeightValues();
	void calcVertexNormals();
	glm::vec3 getNormal(int x, int z) const;
	glm::vec3 getColor(int x, int z) const;
	void determineColors();
	void createVAO();
	float linearInterpolation(float pointA, float pointB, float weightB);

	void LoadingGreyScaleImage();
	bool existingCoord(int x, int z);
};
