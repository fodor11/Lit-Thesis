#pragma once
#include "../src/tdogl/Program.h"
#include <glm/glm.hpp>

class Light {
public:
	/// creates light, sets uniforms in shaderprogram
	Light(std::vector<tdogl::Program*>& allShaders, glm::vec3 position, glm::vec3 ambientIntensities, glm::vec3 diffuseIntensities, glm::vec3 specularIntensities);
	/// updates position, sets light.position uniform in shaderprogram
	void updatePosition(glm::vec3 newPosition);
	/// updates ambient intensities, sets light.ambientIntensities uniform in shaderprogram
	void updateAmbient(glm::vec3 newIntensities);
	/// updates diffuse intensities, sets light.diffuseIntensities uniform in shaderprogram
	void updateDiffuse(glm::vec3 newIntensities);
	/// updates specular intensities, sets light.specularIntensities uniform in shaderprogram
	void updateSpecular(glm::vec3 newIntensities);
	/// updates intensities and position, sets uniforms in shaderprogram
	void updateLight(glm::vec3 newPosition, glm::vec3 ambientIntensities, glm::vec3 diffuseIntensities, glm::vec3 specularIntensities);
	/// returns the position of the lightsource
	glm::vec3 getPosition();
	/// returns the ambient intensities of the lightsource
	glm::vec3 getAmbientIntensities();
	/// returns the diffuse intensities of the lightsource
	glm::vec3 getDiffuseIntensities();
	/// returns the specular intensities of the lightsource
	glm::vec3 getSpecularIntensities();

private:
	glm::vec3 m_vPosition;
	glm::vec3 m_vAmbientIntensities;
	glm::vec3 m_vDiffuseIntensities;
	glm::vec3 m_vSpecularIntensities;
	std::vector<tdogl::Program*> m_vAllShaders;
	void setShaderUniforms();
};