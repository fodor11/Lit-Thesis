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

	/// Sets the angle of the rotation along the Y axis
	void setRotation(float radians);

	/// Returns the position of the fire
	glm::vec3 getPosition();

	/// Increase speed of the fire
	void IncreaseSpeed();

	/// Decrease speed of the fire
	void DecreaseSpeed();

private:
	tdogl::Program* m_pFireShader;
	Camera* m_pCamera;

	glm::mat4 m_mModel;
	glm::vec3 m_vPosition;
	float m_fScale;
	bool m_bHas2planes;
	float m_fElapsedTime = 0.f;
	float m_fAnimationSpeed = 3.0f;

	GLuint m_iTextureID;

	GLuint m_iFireVAO;
	GLuint m_iFireVBO;

	GLuint m_iFireSecondaryVAO;
	GLuint m_iFireSecondaryVBO;

	void loadVAO();

	void drawNormalVAO();
	void drawSecondary1VAO();
	void drawSecondary2VAO();

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
	SpriteFire(tdogl::Program* shaderProgram, Camera* camera, glm::vec3 position, float scale = 1.f);
	/// Empty
	~SpriteFire();

	/// Draws fire
	void drawFire();

	/// Returns the position of the fire
	glm::vec3 getPosition();

	/// Increase speed of the fire
	void IncreaseSpeed();

	/// Decrease speed of the fire
	void DecreaseSpeed();

private:
	BillboardFire* m_pBillboardFire;

	/// Calculates the angle (in radians) to rotate the fire, so that it faces the camera
	float calculateRotation();
	glm::vec3 m_vPosition;
	Camera* m_pCamera;
};