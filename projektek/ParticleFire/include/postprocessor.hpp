#pragma once

#include <random>
#include "../src/tdogl/Program.h"
#include "camera.hpp"
#include <glm/glm.hpp>

class PostProcessor
{
public:
	PostProcessor(int screenWidth, int screenHeigth);
	~PostProcessor();
	void startRenderingOnTexture();
	void stopRenderingOnTexture();
	void draw();

	void reshape(int newWidth, int newHeigth);
private:
	GLuint m_iFbo;
	GLuint m_iFbo_texture;
	GLuint m_iRbo_depth;

	GLuint m_iVerticesVBO;
	GLuint m_iQuadVAO;

	tdogl::Program* m_pShader;

	int m_iScreenWidth;
	int m_iScreenHeigth;

	void initializeFBO();
	void generateVAO();
};
