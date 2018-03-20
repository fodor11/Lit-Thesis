#include "../include/environment.hpp"

Environment::Environment()
{
}

Environment::~Environment()
{
	delete m_pSky;
}

void Environment::initialize(HeightMapLoader* heightMap, Camera* camera, tdogl::Program* skyShaderProgram, std::vector<tdogl::Program*>& allShaders)
{
	m_pCamera = camera;
	m_pHeightmap = heightMap;
	m_vAllShaders = allShaders;
	m_pSkyShader = skyShaderProgram;

	m_pSky = new Sky();
	m_pSky->initialize(m_pSkyShader, m_vAllShaders);
}

void Environment::update()
{
	m_pSky->updateSky(m_pCamera->getX(), m_pCamera->getY(), m_pCamera->getZ(), m_pCamera->getElapsedTime());
}

//TODO: tell the shaders
void Environment::changeAmbientLight(float value)
{
	m_fOverallLightness += value;
	if (m_fOverallLightness > 1.0f)
	{
		m_fOverallLightness = 1.0f;
	}
	else if (m_fOverallLightness < 0.0f)
	{
		m_fOverallLightness = 0.0f;
	}
	m_vLightModelAmbient[0] = m_fOverallLightness;
	m_vLightModelAmbient[1] = m_fOverallLightness;
	m_vLightModelAmbient[2] = m_fOverallLightness;
}

void Environment::toggleMoonlight()
{
	m_pSky->toggleMoonLight();
}

float Environment::calcDistanceToCamera(vec3f position)
{
	vec3f camPos(m_pCamera->getX(), m_pCamera->getY(), m_pCamera->getZ());
	vec3f dist = camPos - position;
	return dist.length();
}