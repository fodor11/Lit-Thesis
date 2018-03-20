#pragma once
#include "sky.hpp"
#include "camera.hpp"
#include "light.hpp"
#include "../src/tdogl/Program.h"

class Environment
{
public:
	/// empty
	Environment();
	/// empty
	~Environment();
	/// reads and prepares objects and textures (sky, forest, trees, terrain)
	void initialize(HeightMapLoader* heightMap, Camera* camera, tdogl::Program* skyShaderProgram, std::vector<tdogl::Program*>& allShadersWithLight);
	/// draws everything
	void update();
	/// TODO: adds the value to the overall lightness
	void changeAmbientLight(float value);
	/// TODO: toggles moon movement on and off
	void toggleMoonlight();

private:
	Sky *m_pSky;
	tdogl::Program *m_pSkyShader;
	std::vector<tdogl::Program*> m_vAllShaders;

	float m_fVisualRange = 6.5f;

	float m_fOverallLightness = 0.1;
	glm::vec3 m_vLightModelAmbient = glm::vec3( m_fOverallLightness, m_fOverallLightness, m_fOverallLightness);

	HeightMapLoader* m_pHeightmap;
	Camera* m_pCamera;

	float calcDistanceToCamera(vec3f position);
};