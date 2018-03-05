#pragma once
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <glm/glm.hpp>
#include "texture.hpp"
#include "light.hpp"
#include "../src/tdogl/Program.h"

class Sky {
public:
	/// empty
	Sky();
	/// deletes VBO-s
	~Sky();
	/// init spheres, light
	void initialize(tdogl::Program *skyShaderProgram, std::vector<tdogl::Program*>& allShaders);
	/// updates datas, draws sky and moon, replaces moonlight
	void updateSky(float cameraX, float cameraY, float cameraZ, float elapsedTime);
	void toggleMoonLight();

private:
	unsigned int m_uiSkyTextureId = 0;
	GLuint m_uiSkyVAO;
	GLuint m_uiSkyIBO;
	GLuint m_uiSkyVBO;
	unsigned int m_uiNumberOfSkyIndicies;
	glm::mat4 m_mSkyModel = glm::mat4();
	glm::vec3 m_vSkyAmbient = glm::vec3(0.5f, 0.5f, 0.5f);
	glm::vec3 m_vSkyDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
	glm::vec3 m_vSkySpecular = glm::vec3(0.5f, 0.5f, 0.5f);

	unsigned int m_uiMoonTextureId = 0;
	GLuint m_uiMoonVAO;
	GLuint m_uiMoonIBO;
	GLuint m_uiMoonVBO;
	unsigned int m_uiNumberOfMoonIndicies;
	glm::mat4 m_mMoonModel = glm::mat4();
	glm::vec3 m_MoonMaterial = glm::vec3(1.f, 1.f, 1.f);
	float m_fMoonSpeed = 0.001f;
	float m_fMoonCurvePosition = 1.5f;
	float m_fSemiAxleX = 300.f;
	float m_fSemiAxleY = 200.f;

	std::vector<GLuint> m_vVBOs;

	Light *m_pLight;
	tdogl::Program *m_pSkyShader;
	std::vector<tdogl::Program*> m_vAllShaders;
	bool m_bMoonlight = true;
	glm::vec3 m_LightPosition = glm::vec3(-50.f, 450.f, 120.f);


	void loadTextures();
	void createSkyVAO();
	void createMoonVAO();
	void drawMoon();
	void drawSky();
	void moveMoon(float elapsedTime);
};
