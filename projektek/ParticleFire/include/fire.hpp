#pragma once

#include <random>
#include "../src/tdogl/Program.h"
#include "camera.hpp"
#include <glm/glm.hpp>

class FireParticle 
{
public:
	FireParticle();

	//FireParticle(const FireParticle& other); // needs update anyway

	FireParticle(glm::vec3 startingPosition, glm::vec3 speedDirection, float rotation, float lifeTime, Camera* camera);

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
	GLubyte getCurrentTexture();
	float getCurrentAlpha();

	static const int textureRowCount = 8;
	
private:

	Camera * m_pCamera;

	float m_fLifeTime;
	float m_fDecayRate = 10.f;
	float m_fAge = 0.f;

	glm::vec3 m_vPosition;

	glm::vec3 m_vSlowing = glm::vec3(-0.05f, -0.01f, -0.05f);
	glm::vec3 m_vSpeedDirection;
	float m_fSpeedRate = 1.f;

	float m_fDistanceToCamera;

	float m_fScale = 1.f;
	float m_fScaleRate = 0.1f;

	GLubyte m_cNumberOfTextures = textureRowCount * textureRowCount - 1; // 2 textures are used at the same time, last texture is only for the fade out effect, no need to display it on its own
	int m_cCurrentTexture;
	float m_fSceneTime; // milliseconds, that 1 texturescene is alive
	float m_fCurrentAlpha;
	glm::vec4 m_vColor;

	float m_fRotation = 0.f;
};

class FireParticleSystem 
{
public:
	FireParticleSystem(Camera * camera, tdogl::Program* fireShader, glm::vec3 position, int maxParticles = 10000, float scale = 1.f);
	// TODO: particle container cleanup
	~FireParticleSystem();
	void draw();
private:
	Camera * m_pCamera;
	tdogl::Program* m_pFireShader;

	FireParticle* m_pParticleContainer;

	glm::vec3 m_vPosition;

	int m_iMaxParticles;
	int m_iNumberOfParticles = 0;
	float m_fParticlesPerSecond = 200.f;
	int m_iParticleLifetime = 1000;	// ms
	float m_fTimeSinceLastEmittedParticle = 0.f;

	float m_fScale;

	GLuint m_iParticleVAO;
	// base mesh
	GLuint m_iVertexVBO;
	// center positions and sizes of the particles
	GLuint m_iPositionsVBO;
	int m_iPositionElementCount = 4; // x,y,z, size
	float *m_pPositionsBuffer;
	// texture number
	GLuint m_iTexturesVBO;
	int *m_pTexturesBuffer;
	GLuint m_iTextureID;
	// rotations and current texture alpha
	GLuint m_iRotationAndAlphaVBO;
	int m_iRotationAndAlphaElements = 2;
	float *m_pRotationAndAlphaBuffer;

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
	void addParticle(float elapsedTime);
};