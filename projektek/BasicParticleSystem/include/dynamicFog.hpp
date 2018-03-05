#pragma once
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "camera.hpp"

class DynamicFog
{
public:
	/// set up fog functions
	DynamicFog(Camera * cameraPointer);
	/// empty
	~DynamicFog();
	/// changes density if needed
	void updateFog();
	/// toggles fog on and off
	void toggleFog();

private:
	/// true if fog is toggled on
	bool m_bFog = false;
	/// true if fog density needs to be changed
	bool m_bFogFading = false;

	float m_fFogDensity = 0.0f;
	float m_fFogMaxDensity = 0.03f;
	float m_fFogDynamicSpeed = 0.00005f;

	/// (on = true) means that the fog is toggled on, so the density might has to be increased
	void changingFogDensity(bool on);

	Camera * m_pCamera;
};