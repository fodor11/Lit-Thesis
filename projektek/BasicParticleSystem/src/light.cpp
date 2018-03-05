#include "../include/light.hpp"

Light::Light(std::vector<tdogl::Program*>& allShaders, glm::vec3 position, glm::vec3 ambientIntensities, glm::vec3 diffuseIntensities, glm::vec3 specularIntensities)
{
	m_vPosition = position;
	m_vAllShaders = allShaders;
	m_vAmbientIntensities = ambientIntensities;
	m_vDiffuseIntensities = diffuseIntensities;
	m_vSpecularIntensities = specularIntensities;
	setShaderUniforms();
}

void Light::updatePosition(glm::vec3 newPosition)
{
	m_vPosition = newPosition;
	for each (tdogl::Program* shader in m_vAllShaders)
	{
		shader->use();
		shader->setUniform("light.position", m_vPosition);
		shader->stopUsing();
	}
}

void Light::updateAmbient(glm::vec3 newIntensities)
{
	m_vAmbientIntensities = newIntensities;
	for each (tdogl::Program* shader in m_vAllShaders)
	{
		shader->use();
		shader->setUniform("light.ambientIntensities", m_vAmbientIntensities);
		shader->stopUsing();
	}
}
void Light::updateDiffuse(glm::vec3 newIntensities)
{
	m_vDiffuseIntensities = newIntensities;
	for each (tdogl::Program* shader in m_vAllShaders)
	{
		shader->use();
		shader->setUniform("light.diffuseIntensities", m_vDiffuseIntensities);
		shader->stopUsing();
	}
}
void Light::updateSpecular(glm::vec3 newIntensities)
{
	m_vSpecularIntensities = newIntensities;
	for each (tdogl::Program* shader in m_vAllShaders)
	{
		shader->use();
		shader->setUniform("light.specularIntensities", m_vSpecularIntensities);
		shader->stopUsing();
	}
}

void Light::updateLight(glm::vec3 newPosition, glm::vec3 ambientIntensities, glm::vec3 diffuseIntensities, glm::vec3 specularIntensities)
{
	m_vPosition = newPosition; 
	m_vAmbientIntensities = ambientIntensities;
	m_vDiffuseIntensities = diffuseIntensities;
	m_vSpecularIntensities = specularIntensities;
	setShaderUniforms();
}

glm::vec3 Light::getPosition()
{
	return m_vPosition;
}

glm::vec3 Light::getAmbientIntensities()
{
	return m_vAmbientIntensities;
}
glm::vec3 Light::getDiffuseIntensities()
{
	return m_vDiffuseIntensities;
}
glm::vec3 Light::getSpecularIntensities()
{
	return m_vSpecularIntensities;
}

void Light::setShaderUniforms()
{
	for each (tdogl::Program* shader in m_vAllShaders)
	{
		shader->use();
		shader->setUniform("light.position", m_vPosition);
		shader->setUniform("light.ambientIntensities", m_vAmbientIntensities);
		shader->setUniform("light.diffuseIntensities", m_vDiffuseIntensities);
		shader->setUniform("light.specularIntensities", m_vSpecularIntensities);
		shader->stopUsing();
	}
}
