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

FireParticle::FireParticle(glm::vec3 startingPosition, glm::vec3 speedDirection, float rotation, float lifeTime, Camera* camera, float rotationRate, float speedRate)
{
	m_pCamera = camera;
	m_vPosition = startingPosition;
	m_vSpeedDirection = speedDirection;
	m_fRotation = rotation;
	m_fLifeTime = lifeTime;
	m_fAge = m_fLifeTime;
	m_vColor = glm::vec4(0.f, 0.f, 0.f, 0.f);
	m_fSceneTime = lifeTime / (float)m_cNumberOfTextures;
	m_fSceneTime *= m_fFireTimeRatio; // quarter of its life is fire, three-quarter of its life is smoke
	m_fRotationRate = rotationRate;
	m_fSpeedRate = speedRate;
}

bool FireParticle::isAlive()
{
	return m_fAge > 0.f;
}

bool FireParticle::update(float elapsedTime)
{
	// decay
	m_fAge -= elapsedTime;

	// check if still alive
	if (m_fAge > 0.f)
	{
		float step = elapsedTime * 0.01f;

		// Ageing
		float relativeAge = m_fAge / m_fLifeTime;
		
		// Slowing x and z much faster than y
		m_vSpeedDirection.x /= 1.f + step * 0.05f * relativeAge;
		m_vSpeedDirection.z /= 1.f + step * 0.05f * relativeAge;
		m_fSpeedRate /= 1.f + step * 0.01 * (1-relativeAge);

		// extra forces
		if (m_bForceApplied)
		{
			m_vPosition += m_vForceDirection * m_fForceSpeed * step;
			m_bForceApplied = false;
		}

		// Moving
		m_vPosition += m_vSpeedDirection * step * m_fSpeedRate;

		// Expanding
		m_fScale += m_fScaleRate * step;

		// Rotate
		m_fRotation += step * m_fRotationRate;

		// Changing texture
		float textureInfo = (m_fLifeTime - m_fAge) / m_fSceneTime;
		m_cCurrentTexture = (int)textureInfo;
		if (m_cCurrentTexture < m_cNumberOfTextures)
		{
			// Flame
			m_fCurrentBlend = 1.f - fmod(textureInfo, 1.0f);
		}
		else
		{
			// Smoke
			m_cCurrentTexture = m_cNumberOfTextures;
			// fade the somke out
			m_fCurrentBlend = relativeAge / (1 - m_fFireTimeRatio);
			m_fScale += 0.1 *step;
		}
		m_fDistanceToCamera = glm::distance2(m_vPosition, m_pCamera->getPosition());
		return true;
	}
	return false;
}

float FireParticle::getDistanceToCamera() const
{
	return m_fDistanceToCamera;
}

void FireParticle::applyForce(glm::vec3 direction, float speed)
{
	m_vForceDirection = direction;
	m_fForceSpeed = speed;
	m_bForceApplied = true;
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

GLubyte FireParticle::getCurrentTexture()
{
	return m_cCurrentTexture;
}

float FireParticle::getCurrentBlend()
{
	return m_fCurrentBlend;
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
	m_pRotationAndBlendBuffer = new float[maxParticles * m_iRotationAndBlendElements];
	m_pTexturesBuffer = new int[maxParticles];
	m_fParticlesPerSecond = ((float)m_iMaxParticles / ((float)m_iParticleLifetime / 1000.f));

	std::cout << "max: " << m_iMaxParticles << std::endl;
	std::cout << "life: " << (float)m_iParticleLifetime/1000.f << std::endl;
	std::cout << "persec: " << m_fParticlesPerSecond << std::endl;

	std::random_device randDev;
	m_rGenerator = std::mt19937(randDev());
	m_rRandomY = std::uniform_real_distribution<float>(0.5f, 1.0f);
	m_rRandomXZ = std::uniform_real_distribution<float>(-0.3f, 0.3f);
	m_rRandomAngle = std::uniform_real_distribution<float>(0.f, 2 * M_PI);
	m_rRandomRadius = std::uniform_real_distribution<float>(0.f, 0.5f);
	m_rRandomRotation = std::uniform_real_distribution<float>(-M_1_PI, M_1_PI);

	loadBaseVAO();
}

FireParticleSystem::~FireParticleSystem()
{
	delete[] m_pParticleContainer;
	delete[] m_pRotationAndBlendBuffer;
	delete[] m_pPositionsBuffer;
	delete[] m_pTexturesBuffer;
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
	m_pFireShader->setUniform("viewProjection", m_pCamera->getProjectionMatrix() * viewMatrix);
	m_pFireShader->setUniform("model", modelMatrix);
	m_pFireShader->setUniform("scale", m_fScale);
	m_pFireShader->setUniform("rowCount", FireParticle::textureRowCount);

	// set texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_iTextureID);
	m_pFireShader->setUniform("tex", 0); //set 0, because it is bound to GL_TEXTURE0
	if (m_iBackgroundTexture != NULL)
	{
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, m_iBackgroundTexture);
		m_pFireShader->setUniform("backgroundDepth", 1);
		m_pFireShader->setUniform("distanceToCamera", getDistance());
	}
	
	//draw
	glDepthFunc(GL_ALWAYS); // blend is not quite right when 2 particles are close (like same z coord etc.), but we are still writing depth values
	//glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBindVertexArray(m_iParticleVAO);
	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, m_iNumberOfParticles);
	glDisable(GL_BLEND);
	//glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
	m_pFireShader->stopUsing();
}

void FireParticleSystem::toggleWind()
{
	if (m_bWindIsBlowing)
	{
		m_bStoppingWind = !m_bStoppingWind;
	}
	else
	{
		m_vWindDirection = glm::vec3(0.5f, 0.f, 0.5f);
		m_bWindIsBlowing = true;
	}
}

float FireParticleSystem::getDistance()
{
	return glm::distance(m_pCamera->getPosition(), m_vPosition);
}

void FireParticleSystem::addBackgroundDepth(GLuint texture)
{
	m_iBackgroundTexture = texture;
}

void FireParticleSystem::updateScreenSize(int width, int heigth)
{
	m_pFireShader->use();
	m_pFireShader->setUniform("screenWidth", 860.f);
	m_pFireShader->setUniform("screenHeigth", 640.f);
	m_pFireShader->stopUsing();
}

void FireParticleSystem::loadBaseVAO()
{
	// Load fire spritesheet
	TextureLoader* textureLoader = new TextureLoader();
	textureLoader->loadMipMappedTexture("particleSpriteSheet.png");
	m_iTextureID = textureLoader->textureID();
	textureLoader->~TextureLoader();

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
							
    // rotations and alphas
	glGenBuffers(1, &m_iRotationAndBlendVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_iRotationAndBlendVBO);
	glBufferData(GL_ARRAY_BUFFER, m_iMaxParticles * m_iRotationAndBlendElements * sizeof(float), NULL, GL_STREAM_DRAW);
	glEnableVertexAttribArray(m_pFireShader->attrib("angleAndBlend"));
	glVertexAttribPointer(m_pFireShader->attrib("angleAndBlend"), m_iRotationAndBlendElements, GL_FLOAT, GL_FALSE, 0, NULL);
	glVertexAttribDivisor(m_pFireShader->attrib("angleAndBlend"), 1);  // 1, because 1 per particle

	// texturesbuffer
	glGenBuffers(1, &m_iTexturesVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_iTexturesVBO);
	glBufferData(GL_ARRAY_BUFFER, m_iMaxParticles * sizeof(int), NULL, GL_STREAM_DRAW);
	glEnableVertexAttribArray(m_pFireShader->attrib("textureNumber"));
	glVertexAttribPointer(m_pFireShader->attrib("textureNumber"), 1, GL_FLOAT, GL_FALSE, 0, NULL); // GL_FLOAT, because glsl is doing some weird stuff
	glVertexAttribDivisor(m_pFireShader->attrib("textureNumber"), 1);  // 1, because 1 per particle
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

void FireParticleSystem::update()
{
	float elapsedTime = m_pCamera->getElapsedTime();

	m_fTimeSinceLastEmittedParticle += elapsedTime;
	int newParticlesCount = (int)(m_fTimeSinceLastEmittedParticle * 0.001f * m_fParticlesPerSecond);

	calculateWindForce(elapsedTime);

	for (int i = 0; i < m_iNumberOfParticles; i++)
	{
		FireParticle &fp = m_pParticleContainer[i];
		if (m_bWindIsBlowing)
		{
			fp.applyForce(m_vWindDirection, m_fWindSpeed);
		}

		if (!fp.update(elapsedTime))
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
	if (m_iNumberOfParticles > 1)
	{
		// noo need to sort, if additive blending is used, but its needed in postprocessing
		std::sort(&m_pParticleContainer[0], &m_pParticleContainer[m_iNumberOfParticles]); //sort range: [container.begin(), container.end()[
	}

	// update positions buffer
	for (int i = 0; i < m_iNumberOfParticles; i++)
	{
		FireParticle &fp = m_pParticleContainer[i];
		m_pPositionsBuffer[i*m_iPositionElementCount] = fp.getX();
		m_pPositionsBuffer[i*m_iPositionElementCount + 1] = fp.getY();
		m_pPositionsBuffer[i*m_iPositionElementCount + 2] = fp.getZ();
		m_pPositionsBuffer[i*m_iPositionElementCount + 3] = fp.getScale();

		m_pRotationAndBlendBuffer[i*m_iRotationAndBlendElements] = fp.getRotation();
		m_pRotationAndBlendBuffer[i*m_iRotationAndBlendElements + 1] = fp.getCurrentBlend();

		m_pTexturesBuffer[i] = fp.getCurrentTexture();
	}

	glBindBuffer(GL_ARRAY_BUFFER, m_iPositionsVBO);
	glBufferData(GL_ARRAY_BUFFER, m_iMaxParticles * m_iPositionElementCount * sizeof(float), NULL, GL_STREAM_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, m_iMaxParticles * m_iPositionElementCount * sizeof(float), m_pPositionsBuffer);

	glBindBuffer(GL_ARRAY_BUFFER, m_iRotationAndBlendVBO);
	glBufferData(GL_ARRAY_BUFFER, m_iMaxParticles * m_iRotationAndBlendElements * sizeof(float), NULL, GL_STREAM_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, m_iMaxParticles * m_iRotationAndBlendElements * sizeof(float), m_pRotationAndBlendBuffer);

	glBindBuffer(GL_ARRAY_BUFFER, m_iTexturesVBO);
	glBufferData(GL_ARRAY_BUFFER, m_iMaxParticles * sizeof(int), NULL, GL_STREAM_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, m_iMaxParticles * sizeof(int), m_pTexturesBuffer);
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
		float radius = m_rRandomRadius(m_rGenerator);
		// makes them start from within a circle (helical coords), instead of 1 point
		glm::vec3 offset = glm::vec3(m_fScale * cos(alpha) * radius, 0.f, m_fScale * sin(alpha) * radius);
		// firewall
		//glm::vec3 offset = glm::vec3(4*radius, 0.f,0.f);
		
		m_pParticleContainer[m_iNumberOfParticles] = FireParticle(m_vPosition + offset, initialSpeed, alpha, m_iParticleLifetime, m_pCamera, m_rRandomRotation(m_rGenerator));
		m_pParticleContainer[m_iNumberOfParticles].update(0.f); // to get the distance from camera
		m_iNumberOfParticles++;
		m_fTimeSinceLastEmittedParticle = 0.f;
	}
}

void FireParticleSystem::calculateWindForce(float elapsedTime)
{
	if (m_bWindIsBlowing)
	{
		// wind is either slowing down or accelerating
		if (m_fWindSpeed < 0.5f || m_bStoppingWind)
		{
			float step = elapsedTime * 0.005f;
			// slowing wind to stop
			if (m_bStoppingWind && m_fWindSpeed > 0.f)
			{
				m_fWindSpeed -= 0.05 * step;
			}
			// turn off wind
			else if (m_fWindSpeed < 0.f)
			{
				m_bWindIsBlowing = false;
				m_bStoppingWind = false;
				m_fWindSpeed = 0.f;
			}
			// accelerating wind
			else
			{
				m_fWindSpeed += 0.05 * step;
			}
		}
	}
}
