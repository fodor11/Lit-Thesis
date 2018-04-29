#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif

#include <math.h>
#include <glm/gtc/matrix_transform.hpp>
#include "../include/sky.hpp"
#include "../include/sphere.hpp"

Sky::Sky()
{
}

Sky::~Sky()
{
	glDeleteBuffers(m_vVBOs.size(), &m_vVBOs.front());
}

void Sky::initialize(tdogl::Program *skyShaderProgram, std::vector<tdogl::Program*>& allShaders)
{
	m_pSkyShader = skyShaderProgram;
	m_vAllShaders = allShaders;
	createSkyVAO();
	createMoonVAO();

	// Light
	glm::vec3 moonLight_ambient = glm::vec3(0.5f, 0.5f, 0.6f);
	glm::vec3 moonLight_diffuse = glm::vec3(0.5f, 0.5f, 0.6f);
	glm::vec3 moonLight_specular = glm::vec3(0.5f, 0.5f, 0.5f);
	m_pLight = new Light(m_vAllShaders, m_LightPosition, moonLight_ambient, moonLight_diffuse, moonLight_specular);

	loadTextures();
}

void Sky::updateSky(float cameraX, float cameraY, float cameraZ, float elapsedTime)
{
	if (m_bMoonlight)
	{
		moveMoon(elapsedTime);
	}
	drawMoon();
	m_mSkyModel = glm::translate(glm::mat4(), glm::vec3(cameraX, cameraY, cameraZ));
	drawSky();

	//reset position of the lightsource
	m_pLight->updatePosition(m_LightPosition);
}

void Sky::toggleMoonLight()
{
	if (m_bMoonlight)
	{
		//glDisable(m_MoonLightId);
		m_bMoonlight = false;
	}
	else
	{
		//glEnable(m_MoonLightId);
		m_bMoonlight = true;
	}
}

void Sky::loadTextures()
{
	cout << "Loading sky textures..." << endl;
	TextureLoader *txtrLoaderObj = new TextureLoader();
	//sky
	txtrLoaderObj->loadTexture("sky5.png");
	m_uiSkyTextureId = txtrLoaderObj->textureID();
	//moon
	txtrLoaderObj->loadMipMappedTexture("moon2.jpg");
	m_uiMoonTextureId = txtrLoaderObj->textureID();
	delete txtrLoaderObj;
}

void Sky::createSkyVAO()
{
	// create and bind the VAO
	glGenVertexArrays(1, &m_uiSkyVAO);
	glBindVertexArray(m_uiSkyVAO);

	Sphere skySphere(500.0f, 64, 64, false);

	// create and bind the VBO for vertices
	glGenBuffers(1, &m_uiSkyVBO);
	m_vVBOs.push_back(m_uiSkyVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_uiSkyVBO);
	glBufferData(GL_ARRAY_BUFFER, skySphere.getVertices().size() * sizeof(glm::vec3), &skySphere.getVertices().front(), GL_STATIC_DRAW);
	// connect the xyz to the "vert" attribute of the vertex shader
	glEnableVertexAttribArray(m_pSkyShader->attrib("vert"));
	glVertexAttribPointer(m_pSkyShader->attrib("vert"), 3, GL_FLOAT, GL_FALSE, 0, NULL);

	// create and bind the VBO for texture coordinates
	glGenBuffers(1, &m_uiSkyVBO);
	m_vVBOs.push_back(m_uiSkyVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_uiSkyVBO);
	glBufferData(GL_ARRAY_BUFFER, skySphere.getTextureCoordinates().size() * sizeof(glm::vec2), &skySphere.getTextureCoordinates().front(), GL_STATIC_DRAW);
	// connect the uv coords to the "vertTexCoord" attribute of the vertex shader
	glEnableVertexAttribArray(m_pSkyShader->attrib("vertTexCoord"));
	glVertexAttribPointer(m_pSkyShader->attrib("vertTexCoord"), 2, GL_FLOAT, GL_FALSE, 0, NULL);

	// create and bind the VBO for normals
	glGenBuffers(1, &m_uiSkyVBO);
	m_vVBOs.push_back(m_uiSkyVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_uiSkyVBO);
	glBufferData(GL_ARRAY_BUFFER, skySphere.getNormals().size() * sizeof(glm::vec3), &skySphere.getNormals().front(), GL_STATIC_DRAW);
	// connect the normal to the "vertNorm" attribute of the vertex shader
	glEnableVertexAttribArray(m_pSkyShader->attrib("vertNorm"));
	glVertexAttribPointer(m_pSkyShader->attrib("vertNorm"), 3, GL_FLOAT, GL_TRUE, 0, NULL);

	// create the IBO
	m_uiNumberOfSkyIndicies = skySphere.getIndicies().size() * 3;
	glGenBuffers(1, &m_uiSkyIBO);
	m_vVBOs.push_back(m_uiSkyIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_uiSkyIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_uiNumberOfSkyIndicies * sizeof(int), &skySphere.getIndicies().front(), GL_STATIC_DRAW);

	// unbind the VAO
	glBindVertexArray(0);
}

void Sky::createMoonVAO()
{	
	// create and bind the VAO
	glGenVertexArrays(1, &m_uiMoonVAO);
	glBindVertexArray(m_uiMoonVAO);

	Sphere moonSphere(25.f, 32, 32, true);

	// create and bind the VBO for vertices
	glGenBuffers(1, &m_uiMoonVBO);
	m_vVBOs.push_back(m_uiMoonVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_uiMoonVBO);
	glBufferData(GL_ARRAY_BUFFER, moonSphere.getVertices().size() * sizeof(glm::vec3), &moonSphere.getVertices().front(), GL_STATIC_DRAW);
	// connect the xyz to the "vert" attribute of the vertex shader
	glEnableVertexAttribArray(m_pSkyShader->attrib("vert"));
	glVertexAttribPointer(m_pSkyShader->attrib("vert"), 3, GL_FLOAT, GL_FALSE, 0, NULL);

	// create and bind the VBO for texture coordinates
	glGenBuffers(1, &m_uiMoonVBO);
	m_vVBOs.push_back(m_uiMoonVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_uiMoonVBO);
	glBufferData(GL_ARRAY_BUFFER, moonSphere.getTextureCoordinates().size() * sizeof(glm::vec2), &moonSphere.getTextureCoordinates().front(), GL_STATIC_DRAW);
	// connect the uv coords to the "vertTexCoord" attribute of the vertex shader
	glEnableVertexAttribArray(m_pSkyShader->attrib("vertTexCoord"));
	glVertexAttribPointer(m_pSkyShader->attrib("vertTexCoord"), 2, GL_FLOAT, GL_FALSE, 0, NULL);

	// create and bind the VBO for normals
	glGenBuffers(1, &m_uiMoonVBO);
	m_vVBOs.push_back(m_uiMoonVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_uiMoonVBO);
	glBufferData(GL_ARRAY_BUFFER, moonSphere.getNormals().size() * sizeof(glm::vec3), &moonSphere.getNormals().front(), GL_STATIC_DRAW);
	// connect the normal to the "vertNorm" attribute of the vertex shader
	glEnableVertexAttribArray(m_pSkyShader->attrib("vertNorm"));
	glVertexAttribPointer(m_pSkyShader->attrib("vertNorm"), 3, GL_FLOAT, GL_TRUE, 0, NULL);

	// create the IBO
	m_uiNumberOfMoonIndicies = moonSphere.getIndicies().size() * 3;
	glGenBuffers(1, &m_uiMoonIBO);
	m_vVBOs.push_back(m_uiMoonIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_uiMoonIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_uiNumberOfMoonIndicies * sizeof(int), &moonSphere.getIndicies().front(), GL_STATIC_DRAW);

	// unbind the VAO
	glBindVertexArray(0);
}

void Sky::drawMoon()
{
	m_mMoonModel = glm::translate(glm::mat4(), glm::vec3(m_LightPosition[0], m_LightPosition[1], m_LightPosition[2]));
	m_mMoonModel = glm::rotate(m_mMoonModel, glm::radians<float>(180.f), glm::vec3(0.f, 1.f, 0.f));
	m_mMoonModel = glm::rotate(m_mMoonModel, glm::radians<float>(45.f), glm::vec3(0.f, 0.f, 1.f));
	m_mMoonModel = glm::rotate(m_mMoonModel, glm::radians<float>(180.f), glm::vec3(1.f, 0.f, 0.f));

	// bind the program (the shaders)
	m_pSkyShader->use();

	// set the "model" uniform in the vertex shader
	m_pSkyShader->setUniform("model", m_mMoonModel);

	// set colors
	m_pSkyShader->setUniform("ambientColor", m_MoonMaterial);
	m_pSkyShader->setUniform("diffuseColor", m_MoonMaterial);
	m_pSkyShader->setUniform("specularColor", m_MoonMaterial);

	// set the "normalMatrix" uniform in the vertex shader
	m_pSkyShader->setUniform("normalMatrix", glm::mat3(glm::transpose(glm::inverse(m_mMoonModel))));

	// bind the texture and set the "tex" uniform in the fragment shader
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_uiMoonTextureId);
	m_pSkyShader->setUniform("tex", 0); //set to 0 because the texture is bound to GL_TEXTURE0
	
	// bind the VAO 
	glBindVertexArray(m_uiMoonVAO);
	// draw the IBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_uiMoonIBO);
	glDrawElements(GL_TRIANGLES, m_uiNumberOfMoonIndicies, GL_UNSIGNED_INT, NULL);
	// unbind the IBO, VAO, program and the texture
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	m_pSkyShader->stopUsing();
}

void Sky::drawSky()
{
	// bind the program (the shaders)
	m_pSkyShader->use();

	// set the "model" uniform in the vertex shader
	m_pSkyShader->setUniform("model", m_mSkyModel);

	// set colors
	m_pSkyShader->setUniform("ambientColor", m_vSkyAmbient);
	m_pSkyShader->setUniform("diffuseColor", m_vSkyDiffuse);
	m_pSkyShader->setUniform("specularColor", m_vSkySpecular);

	// set the "normalMatrix" uniform in the vertex shader
	m_pSkyShader->setUniform("normalMatrix", glm::mat3(glm::transpose(glm::inverse(m_mSkyModel))));

	// bind the texture and set the "tex" uniform in the fragment shader
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_uiSkyTextureId);
	m_pSkyShader->setUniform("tex", 0); //set to 0 because the texture is bound to GL_TEXTURE0

	// bind the VAO 
	glBindVertexArray(m_uiSkyVAO);
	// draw the IBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_uiSkyIBO);
	glDrawElements(GL_TRIANGLES, m_uiNumberOfSkyIndicies, GL_UNSIGNED_INT, NULL);
	// unbind the IBO, VAO, program and the texture
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	m_pSkyShader->stopUsing();
}

void Sky::moveMoon(float elapsedTime)
{
	m_fMoonCurvePosition += m_fMoonSpeed * elapsedTime;
	if (m_fMoonCurvePosition > M_PI)
	{
		m_fMoonCurvePosition = 0.0f;
	}
	//y coordinate
	m_LightPosition[1] = m_fSemiAxleY * sin(m_fMoonCurvePosition);
	//z coordinate
	m_LightPosition[2] = m_fSemiAxleX * cos(m_fMoonCurvePosition);
}
