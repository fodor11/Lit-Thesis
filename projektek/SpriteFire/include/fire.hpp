#pragma once

#include "../src/tdogl/Program.h"
#include "camera.hpp"
#include <glm/glm.hpp>

class BillboardFire {
public:

	/// Creates the VAO (scale will be the exact height of the billboard)
	BillboardFire(tdogl::Program* shaderProgram, Camera* camera, glm::vec3 position, bool has2planes = false, float scale = 1.f);

	/// Empty
	~BillboardFire();

	/// Draws fire
	void drawFire();

private:
	tdogl::Program* m_pFireShader;
	Camera* m_pCamera;

	glm::mat4 m_mModel;
	glm::vec3 m_vPosition;
	float m_fScale;
	bool m_bHas2planes;
	float m_fElapsedTime = 0.f;

	GLuint m_iFireVAO;
	GLuint m_iTextureID;
	GLuint m_iFireVBO;

	void loadVAO();

	/// Calculates the vertices for the billboard
	std::vector<glm::vec3> calculateBillboardVertices();
	int m_iNumberOfVertices;

	/// Calculates the actual texture coordinates for a given frame 
	std::vector<glm::vec2> calculateNextTextureCoordinates();
	int m_iColumns = 8;
	int m_iRows = 4;
	int m_iActualFrame = 0;
	int m_iNumberOfFrames = m_iColumns * m_iRows;
	float m_fRowstep = 1.f / m_iRows;
	float m_fColumnstep = 1.f / m_iColumns;
};

class SpriteFire {
public:
	/// Creates the VAO
	SpriteFire(tdogl::Program* shaderProgram, Camera* camera, glm::vec3 position);

	~SpriteFire();

	/// Draws fire
	void drawFire();

	/// Sets the scale for the fire (scale will be the exact height)
	void setScale(float scale);

private:
	tdogl::Program* m_pFireShader;
	Camera* m_pCamera;
	glm::mat4 m_mModel = glm::mat4();
	glm::vec3 m_vPosition = glm::vec3();
	float m_fScale = 1.f;

	BillboardFire* m_pBillboardFire();
};