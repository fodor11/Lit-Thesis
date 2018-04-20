#include <algorithm>
#include <utility>
#include "../include/fire.hpp"
#include "../include/texture.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>

FireParticle::FireParticle()
{
	m_fLifeTime = 0.f;
	m_fAge = m_fLifeTime;
}

FireParticle::FireParticle(const FireParticle & other)
{
	m_pCamera = other.m_pCamera;
	m_fAge = other.m_fAge;
	m_fDecayRate = other.m_fDecayRate;
	m_fDistanceToCamera = other.m_fDistanceToCamera;
	m_fLifeTime = other.m_fLifeTime;
	m_fRotation = other.m_fRotation;
	m_fScale = other.m_fScale;
	m_vSlowing = other.m_vSlowing;
	m_vColor = other.m_vColor;
	m_vPosition = other.m_vPosition;
	m_vSpeedDirection = other.m_vSpeedDirection;
}

FireParticle::FireParticle(glm::vec3 startingPosition, glm::vec3 speedDirection, float rotation, float lifeTime, Camera* camera)
{
	m_pCamera = camera;
	m_vPosition = startingPosition;
	m_vSpeedDirection = speedDirection;
	m_fRotation = rotation;
	m_fLifeTime = lifeTime;
	m_fAge = m_fLifeTime;
	m_vColor = glm::vec4(0.f, 0.f, 0.f, 0.f);

}

bool FireParticle::isAlive()
{
	return m_fAge > 0.f;
}

bool FireParticle::update(float elapsedTime)
{
	// decay
	m_fAge -= elapsedTime * m_fDecayRate;
	float step = elapsedTime / 10.f;

	// check if still alive
	if (m_fAge > 0.f)
	{
		float relativeAge = m_fAge / m_fLifeTime;
		m_vPosition = m_vPosition + m_vSpeedDirection * step * m_fSpeedRate;
		
		// m_fSpeedRate *= step * 2;
		// slowing x and z much faster than y
		m_vSpeedDirection.x /= 1.f + step * 0.05f * relativeAge;
		m_vSpeedDirection.z /= 1.f + step * 0.05f * relativeAge;

		m_fScale += m_fScaleRate * step * relativeAge;

		m_fDistanceToCamera = glm::distance2(m_vPosition, m_pCamera->getPosition());
		return true;
	}
	return false;
}

float FireParticle::getDistanceToCamera() const
{
	return m_fDistanceToCamera;
}

inline bool FireParticle::operator<(const FireParticle & rhs)
{
	// the further the particle, the earlier it should be drawn
	return m_fDistanceToCamera > rhs.getDistanceToCamera();
}

float FireParticle::getX()
{
	return m_vPosition.x;
}

float FireParticle::getY()
{
	return m_vPosition.y;
}

float FireParticle::getZ()
{
	return m_vPosition.z;
}

float FireParticle::getScale()
{
	return m_fScale;
}

float FireParticle::getRotation()
{
	return m_fRotation;
}

FireParticleSystem::FireParticleSystem(Camera * camera, tdogl::Program * fireShader, glm::vec3 position, int maxParticles, float scale)
{
	m_pCamera = camera;
	m_pFireShader = fireShader;
	m_vPosition = position;
	m_iMaxParticles = maxParticles;
	m_fScale = scale;
	m_pParticleContainer = new FireParticle[maxParticles];
	m_pPositionsBuffer = new float[maxParticles * m_iPositionElementCount];
	m_pRotationsBuffer = new float[maxParticles];
	m_fParticlesPerSecond = m_iMaxParticles / (m_iParticleLifetime / 1000);
	std::cout << "max: " << m_iMaxParticles << std::endl;
	std::cout << "life: " << m_iParticleLifetime/1000 << std::endl;
	std::cout << "persec: " << m_fParticlesPerSecond << std::endl;
	std::random_device randDev;
	m_rGenerator = std::mt19937(randDev());
	m_rRandomY = std::uniform_real_distribution<float>(0.5f, 1.0f);
	m_rRandomXZ = std::uniform_real_distribution<float>(-0.3f, 0.3f);
	m_rRandomAngle = std::uniform_real_distribution<float>(0.f, M_PI);

	loadBaseVAO();
}

void FireParticleSystem::draw()
{
	update();

	//std::cout << "count: " << m_iNumberOfParticles << std::endl;
	m_pFireShader->use();

	glm::mat4 viewMatrix = m_pCamera->getViewMatrix();
	glm::mat4 modelMatrix = glm::mat4();
	// cancel out the rotations of the camera, so the particles always face the viewer
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			modelMatrix[i][j] = viewMatrix[j][i];
		}
	}
	//modelMatrix = glm::scale(modelMatrix, glm::vec3(m_fScale, m_fScale, m_fScale));
	//m_pFireShader->setUniform("mvpMatrix", m_pCamera->getProjectionMatrix() * viewMatrix * modelMatrix);
	m_pFireShader->setUniform("viewProjection", m_pCamera->getProjectionMatrix() * viewMatrix);
	m_pFireShader->setUniform("model", modelMatrix);
	m_pFireShader->setUniform("scale", m_fScale);


	//TODO: bind textures, send them to shaders

	glBindVertexArray(m_iParticleVAO);

	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, m_iNumberOfParticles);

	glBindVertexArray(0);
	m_pFireShader->stopUsing();
	// unbind textures
}

void FireParticleSystem::loadBaseVAO()
{
	std::vector<glm::vec3> vertices = generateVertices();

	glGenVertexArrays(1, &m_iParticleVAO);
	glBindVertexArray(m_iParticleVAO);

	// vertices
	glGenBuffers(1, &m_iVertexVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_iVertexVBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices.front(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(m_pFireShader->attrib("vert"));
	glVertexAttribPointer(m_pFireShader->attrib("vert"), 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glVertexAttribDivisor(m_pFireShader->attrib("vert"), 0);  // 0, because always these vertices will be drawn

	// positions and sizes
	glGenBuffers(1, &m_iPositionsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_iPositionsVBO);
	glBufferData(GL_ARRAY_BUFFER, m_iMaxParticles * m_iPositionElementCount * sizeof(float), NULL, GL_STREAM_DRAW);
	glEnableVertexAttribArray(m_pFireShader->attrib("positionAndSize"));
	glVertexAttribPointer(m_pFireShader->attrib("positionAndSize"), m_iPositionElementCount, GL_FLOAT, GL_FALSE, 0, NULL);
	glVertexAttribDivisor(m_pFireShader->attrib("positionAndSize"), 1);  // 1, because 1 per particle
							
    // rotations
	glGenBuffers(1, &m_iRotationsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_iRotationsVBO);
	glBufferData(GL_ARRAY_BUFFER, m_iMaxParticles * sizeof(float), NULL, GL_STREAM_DRAW);
	glEnableVertexAttribArray(m_pFireShader->attrib("angle"));
	glVertexAttribPointer(m_pFireShader->attrib("angle"), 1, GL_FLOAT, GL_FALSE, 0, NULL);
	glVertexAttribDivisor(m_pFireShader->attrib("angle"), 1);  // 1, because 1 per particle
}

std::vector<glm::vec3> FireParticleSystem::generateVertices()
{
	// 3 --- 2
	// |  /  |
	// 1 --- 4
	// triangles: 1 + 2 + 3; 1 + 4 + 2
	std::vector<glm::vec3> vertices;

	glm::vec3 first(-0.5f, -0.5f, 0.f);
	glm::vec3 second(0.5f, 0.5f, 0.f);
	glm::vec3 third(-0.5f, 0.5f, 0.f);
	glm::vec3 fourth(0.5f, -0.5f, 0.f);

	vertices.push_back(first);
	vertices.push_back(second);
	vertices.push_back(third);
	vertices.push_back(first);
	vertices.push_back(fourth);
	vertices.push_back(second);

	return vertices;
}

// TODO: check container bounds
void FireParticleSystem::update()
{
	float elapsedTime = m_pCamera->getElapsedTime();
	m_fTimeSinceLastEmittedParticle += elapsedTime;
	int newParticlesCount = (int)m_fTimeSinceLastEmittedParticle * m_fParticlesPerSecond * 0.001f;
	//std::cout << "new particles: "<<newParticlesCount << std::endl;

	for (int i = 0; i < m_iNumberOfParticles; i++)
	{
		if (!m_pParticleContainer[i].update(elapsedTime))
		{
			// if the particle died
			killParticle(i);
			// swap happend, the element at this index should be updated again
			i--;
		}
	}

	// add new particles
	for (int i = 0; i < newParticlesCount; i++)
	{
		addParticle();
	}

	// sort furthest to closest from camera
	if (m_iNumberOfParticles > 0)
	{
		std::sort(&m_pParticleContainer[0], &m_pParticleContainer[m_iNumberOfParticles - 1]);
	}

	// update positions buffer
	for (int i = 0; i < m_iNumberOfParticles; i++)
	{
		FireParticle &fp = m_pParticleContainer[i];
		m_pPositionsBuffer[i*m_iPositionElementCount] = fp.getX();
		m_pPositionsBuffer[i*m_iPositionElementCount + 1] = fp.getY();
		m_pPositionsBuffer[i*m_iPositionElementCount + 2] = fp.getZ();
		m_pPositionsBuffer[i*m_iPositionElementCount + 3] = fp.getScale();

		m_pRotationsBuffer[i] = fp.getRotation();
	}
	glBindVertexArray(m_iParticleVAO); // TODO: is this needed?
	glBindBuffer(GL_ARRAY_BUFFER, m_iPositionsVBO);
	glBufferData(GL_ARRAY_BUFFER, m_iMaxParticles * m_iPositionElementCount * sizeof(float), NULL, GL_STREAM_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, m_iMaxParticles * m_iPositionElementCount * sizeof(float), m_pPositionsBuffer);

	glBindBuffer(GL_ARRAY_BUFFER, m_iRotationsVBO);
	glBufferData(GL_ARRAY_BUFFER, m_iMaxParticles * sizeof(float), NULL, GL_STREAM_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, m_iMaxParticles * sizeof(float), m_pRotationsBuffer);
	glBindVertexArray(0);
}

void FireParticleSystem::killParticle(int index)
{
	std::swap(m_pParticleContainer[index], m_pParticleContainer[m_iNumberOfParticles - 1]);
	m_iNumberOfParticles--;
}

void FireParticleSystem::addParticle()
{
	if (m_iNumberOfParticles < m_iMaxParticles)
	{
		// gives direction and speed
		glm::vec3 initialSpeed(m_rRandomXZ(m_rGenerator), m_rRandomY(m_rGenerator), m_rRandomXZ(m_rGenerator));

		float alpha = m_rRandomAngle(m_rGenerator);

		// makes them start within a circle (helical coords), instead of 1 point
		glm::vec3 offset = glm::vec3(m_fScale * cos(alpha), 0.f, m_fScale * sin(alpha));
		
		m_pParticleContainer[m_iNumberOfParticles] = FireParticle(m_vPosition + offset, initialSpeed, alpha, m_iParticleLifetime, m_pCamera);
		m_iNumberOfParticles++;
		m_fTimeSinceLastEmittedParticle = 0.f;
	}
}
