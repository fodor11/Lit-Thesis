#pragma once

#include <random>
#include "../src/tdogl/Program.h"
#include "camera.hpp"
#include <glm/glm.hpp>

class FireParticle 
{
public:
	FireParticle();

	FireParticle(const FireParticle& other);

	FireParticle(glm::vec3 startingPosition, glm::vec3 speed, float rotation, float lifeTime, Camera* camera);

	bool isAlive();

	void rotateOverTime(float rotation, float time);

	bool update(float elapsedTime);

	float getDistanceToCamera() const;

	inline bool operator<(const FireParticle& rhs);

	float getX();
	float getY();
	float getZ();
	float getScale();
	float getRotation();
	
private:

	Camera * m_pCamera;

	float m_fLifeTime;
	float m_fDecayRate = 10.f;
	float m_fAge = 0.f;

	glm::vec3 m_vPosition;

	float m_fSlowingRate = 0.8f;
	glm::vec3 m_vSpeed;

	float m_fDistanceToCamera;

	float m_fScale = 1.f;

	glm::vec4 m_vColor;

	float m_fRotation = 0.f;
};

class FireParticleSystem 
{
public:
	FireParticleSystem(Camera * camera, tdogl::Program* fireShader, glm::vec3 position, int maxParticles = 10000, float scale = 1.f);
	// TODO: particle container cleanup?
	~FireParticleSystem();
	void draw();
private:
	Camera * m_pCamera;
	tdogl::Program* m_pFireShader;

	FireParticle* m_pParticleContainer;

	glm::vec3 m_vPosition;

	int m_iMaxParticles;
	int m_iNumberOfParticles = 0;
	float m_fParticlesPerSecond = 50.f;
	float m_fTimeSinceLastEmittedParticle = 0.f;

	float m_fScale;

	GLuint m_iParticleVAO;
	// base mesh
	GLuint m_iVertexVBO;
	// center positions and sizes of the particles
	GLuint m_iPositionsVBO;
	int m_iPositionElementCount = 4; // x,y,z, size
	float *m_pPositionsBuffer;
	// TODO: texture coordiantes
	GLuint m_iTexturesVBO;
	int m_iTexturesElementCount = 2;
	float *m_pTexturesBuffer;
	// rotations
	GLuint m_iRotationsVBO;
	float *m_pRotationsBuffer;

	// random
	std::mt19937 m_rGenerator;
	std::uniform_real_distribution<float> m_rRandomY;
	std::uniform_real_distribution<float> m_rRandomXZ;
	std::uniform_real_distribution<float> m_rRandomAngle;

	// loads the base VAO
	void loadBaseVAO();
	// generates the sprite vertices
	std::vector<glm::vec3> generateVertices();
	// updates particle positions, sorts them
	void update();
	// swaps dead particle with the last active one
	void killParticle(int index);
	// adds new particle to the container
	void addParticle();
};