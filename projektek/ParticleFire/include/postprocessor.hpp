#pragma once

#include <random>
#include "../src/tdogl/Program.h"
#include "camera.hpp"
#include <glm/glm.hpp>

class PostProcessor
{
public:
	PostProcessor(std::string shaderName, int screenWidth, int screenHeigth);
	~PostProcessor();

	void startRenderingOnTexture();
	void stopRenderingOnTexture();

	void draw();

	void setOffset(float offset);
	void addTexture(GLuint textureId);
	void addBackgroundDepth(GLuint textureId);
	void addForegroundDepth(GLuint textureId);
	GLuint getTextureId();
	GLuint getDepthId();

	void reshape(int newWidth, int newHeigth);
private:
	GLuint m_iFbo;
	GLuint m_iFbo_texture;
	GLuint m_iRbo_depth;

	GLuint m_iAdditionalTexture = NULL;
	GLuint m_iBackgroundDepth = NULL;
	GLuint m_iForegroundDepth = NULL;

	GLuint m_iVerticesVBO;
	GLuint m_iQuadVAO;

	tdogl::Program* m_pShader;

	int m_iScreenWidth;
	int m_iScreenHeigth;

	void initializeFBO();
	void generateVAO();
};
