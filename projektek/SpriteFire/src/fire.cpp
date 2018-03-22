#include "../include/fire.hpp"
#include "../include/texture.hpp"
#include <glm/gtc/matrix_transform.hpp>

BillboardFire::BillboardFire(tdogl::Program * shaderProgram, Camera * camera, glm::vec3 position, bool has2planes, float scale)
{
	m_fScale = scale;
	m_pFireShader = shaderProgram;
	m_pCamera = camera;
	m_vPosition = position;
	m_mModel = glm::translate(glm::mat4(), m_vPosition);
	m_iNumberOfVertices = 6;
	m_bHas2planes = has2planes;
	loadVAO();
}

BillboardFire::~BillboardFire()
{
}

void BillboardFire::drawNormalVAO() 
{
	m_pFireShader->setUniform("model", m_mModel);
	glDrawArrays(GL_TRIANGLES, 0, m_iNumberOfVertices);
}
void BillboardFire::drawRotatedVAO() 
{
	m_pFireShader->setUniform("model", glm::rotate(m_mModel, glm::radians(90.f), glm::vec3(0, 1, 0)));
	glDrawArrays(GL_TRIANGLES, 0, m_iNumberOfVertices);
}
void BillboardFire::drawFire()
{
	// set shader
	m_pFireShader->use();

	// set the "model" uniform in the vertex shader
	//m_pFireShader->setUniform("model", m_mModel);

	// set texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_iTextureID);
	m_pFireShader->setUniform("tex", 0); //set 0, because it is bound to GL_TEXTURE0

	// draw
	glBindVertexArray(m_iFireVAO);
	glDisable(GL_DEPTH_TEST);
	
	if (m_bHas2planes)
	{
		if (m_pCamera->getX() < m_vPosition.x && m_pCamera->getZ() < m_vPosition.z || m_pCamera->getX() > m_vPosition.x && m_pCamera->getZ() > m_vPosition.z)
		{
			drawNormalVAO();
			drawRotatedVAO();
		}
		else if (m_pCamera->getX() < m_vPosition.x && m_pCamera->getZ() > m_vPosition.z || m_pCamera->getX() > m_vPosition.x && m_pCamera->getZ() < m_vPosition.z)
		{
			drawRotatedVAO();
			drawNormalVAO();
		}
		else if (m_pCamera->getX() == m_vPosition.x)
		{
			drawRotatedVAO();
		}
		else if (m_pCamera->getZ() == m_vPosition.z)
		{
			drawNormalVAO();
		}
	}
	else
	{
		drawNormalVAO();
	}

	glEnable(GL_DEPTH_TEST);

	// unbind everything
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	m_pFireShader->stopUsing();

	// load next frame
	m_fElapsedTime += m_pCamera->getElapsedTime();
	if (m_fElapsedTime >= 3.0)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_iFireVBO);
		std::vector<glm::vec2> textureCoords = calculateNextTextureCoordinates();
		glBufferSubData(GL_ARRAY_BUFFER, 0, textureCoords.size() * sizeof(glm::vec2), &textureCoords.front());

		m_fElapsedTime = 0;
	}
}

void BillboardFire::setRotation(float radians)
{
	m_mModel = glm::rotate(glm::translate(glm::mat4(), m_vPosition), radians, glm::vec3(0, 1, 0));
}

glm::vec3 BillboardFire::getPosition()
{
	return m_vPosition;
}

void BillboardFire::loadVAO()
{
	// Load fire spritesheet
	TextureLoader* textureLoader = new TextureLoader();
	textureLoader->loadMipMappedTexture("fire.png");
	m_iTextureID = textureLoader->textureID();
	textureLoader->~TextureLoader();

	// Calculate vertices and texture uv-s
	std::vector<glm::vec3> vertices = calculateBillboardVertices();
	std::vector<glm::vec2> textureCoordinates = calculateNextTextureCoordinates();

	// Load vertex array buffers & object
	glGenVertexArrays(1, &m_iFireVAO);
	glBindVertexArray(m_iFireVAO);

	glGenBuffers(1, &m_iFireVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_iFireVBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices.front(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(m_pFireShader->attrib("vert"));
	glVertexAttribPointer(m_pFireShader->attrib("vert"), 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glGenBuffers(1, &m_iFireVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_iFireVBO);
	glBufferData(GL_ARRAY_BUFFER, textureCoordinates.size() * sizeof(glm::vec2), &textureCoordinates.front(), GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(m_pFireShader->attrib("vertTexCoord"));
	glVertexAttribPointer(m_pFireShader->attrib("vertTexCoord"), 2, GL_FLOAT, GL_FALSE, 0, NULL);

	glBindVertexArray(0);
}

std::vector<glm::vec3> BillboardFire::calculateBillboardVertices()
{
	std::vector<glm::vec3> vertices;

	float height = m_fScale;
	float halfWidth = height / 4.f; // the sprite's height/width ratio is 2:1

	// 3 -- 2
	// |  / |
	// | /  |
	// 1 -- 4
	// triangles: 1 + 2 + 3; 1 + 4 + 2
	vertices.push_back(glm::vec3(-halfWidth, 0, 0));     // 1
	vertices.push_back(glm::vec3(halfWidth, height, 0)); // 2
	vertices.push_back(glm::vec3(-halfWidth, height, 0));// 3
	vertices.push_back(glm::vec3(-halfWidth, 0, 0));     // 1
	vertices.push_back(glm::vec3(halfWidth, 0, 0));      // 4
	vertices.push_back(glm::vec3(halfWidth, height, 0)); // 2

	//if (m_bHas2planes)
	//{
	//	// second plane rotated by 90 degrees
	//	vertices.push_back(glm::vec3(0, 0, -halfWidth));     // 1
	//	vertices.push_back(glm::vec3(0, height, halfWidth)); // 2
	//	vertices.push_back(glm::vec3(0, height, -halfWidth));// 3
	//	vertices.push_back(glm::vec3(0, 0, -halfWidth));     // 1
	//	vertices.push_back(glm::vec3(0, 0, halfWidth));      // 4
	//	vertices.push_back(glm::vec3(0, height, halfWidth)); // 2
	//}
	
	return vertices;
}

std::vector<glm::vec2> BillboardFire::calculateNextTextureCoordinates()
{
	// calculate column and row numbers for the actual sprite
	int row = m_iActualFrame / m_iColumns;
	int column = m_iActualFrame % m_iColumns;
	//std::cout << "Frame: " << m_iActualFrame << ", row: " << row << ", column: " << column << std::endl;

	// texture coordinates accordint to the vertices (1 + 2 + 3; 1 + 4 + 2)
	std::vector<glm::vec2> textureCoordinates;

	glm::vec2 first(column * m_fColumnstep, (row + 1) * m_fRowstep);
	glm::vec2 second((column + 1) * m_fColumnstep, row * m_fRowstep);
	glm::vec2 third(column * m_fColumnstep, row * m_fRowstep);
	glm::vec2 fourth((column + 1) * m_fColumnstep, (row + 1) * m_fRowstep);
	//std::cout << "1.: "<< first.x << ", "<< first.y << std::endl;
	//std::cout << "2.: " << second.x << ", " << second.y << std::endl;
	//std::cout << "3.: " << third.x << ", " << third.y << std::endl;
	//std::cout << "4.: " << fourth.x << ", " << fourth.y << std::endl;


	textureCoordinates.push_back(first);   //1
	textureCoordinates.push_back(second);  //2
	textureCoordinates.push_back(third);   //3
	textureCoordinates.push_back(first);   //1
	textureCoordinates.push_back(fourth);  //4
	textureCoordinates.push_back(second);  //2


	//if (m_bHas2planes)
	//{
	//	textureCoordinates.push_back(first);   //1
	//	textureCoordinates.push_back(second);  //2
	//	textureCoordinates.push_back(third);   //3
	//	textureCoordinates.push_back(first);   //1
	//	textureCoordinates.push_back(fourth);  //4
	//	textureCoordinates.push_back(second);  //2
	//}

	// Next frame
	if (++m_iActualFrame == m_iNumberOfFrames)
	{
		m_iActualFrame = 0;
	}

	return textureCoordinates;
}

SpriteFire::SpriteFire(tdogl::Program * shaderProgram, Camera * camera, glm::vec3 position, float scale)
{
	m_pBillboardFire = new BillboardFire(shaderProgram, camera, position, false, scale);
	m_vPosition = position;
	m_pCamera = camera;
}

SpriteFire::~SpriteFire()
{
}

void SpriteFire::drawFire()
{
	m_pBillboardFire->setRotation(calculateRotation());
	m_pBillboardFire->drawFire();
}

glm::vec3 SpriteFire::getPosition()
{
	return m_pBillboardFire->getPosition();
}

float SpriteFire::calculateRotation()
{
	float angle = 0.f;
	glm::vec2 cameraPos(m_pCamera->getX(), m_pCamera->getZ());
	glm::vec2 firePos(m_vPosition.x, m_vPosition.z);

	// the vector pointing from the fire to the camera
	glm::vec2 eye = glm::normalize(cameraPos - firePos);

	float cosAlpha = glm::dot(eye, glm::vec2(0.f, -1.f)); // cosine of the angle with the Z axis

	// Angle between vetors is <= 180, so if the camera's x coordinate is grater than the fire's, 
	// then we need to rotate the fire by (-1) * angle
	if (cameraPos.x < firePos.x)
	{
		angle = glm::acos(cosAlpha);
	}
	else
	{
		angle = glm::acos(-cosAlpha);
	}

	return angle;
}
