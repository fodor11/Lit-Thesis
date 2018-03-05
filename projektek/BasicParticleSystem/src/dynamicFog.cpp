#include "../include/dynamicFog.hpp"

DynamicFog::DynamicFog(Camera * cameraPointer)
{
	m_pCamera = cameraPointer;
	GLfloat fogColor[4] = { 0.12f, 0.12f, 0.12f, 1.0f };
	glFogf(GL_FOG_MODE, GL_EXP);
	glFogfv(GL_FOG_COLOR, fogColor);
	glFogf(GL_FOG_DENSITY, m_fFogDensity);
	glFogf(GL_FOG_START, 1.0f);
	glFogf(GL_FOG_END, 150.0f);
	glHint(GL_FOG_HINT, GL_NICEST);
}

DynamicFog::~DynamicFog(){}

void DynamicFog::updateFog()
{
	if (m_bFogFading)
	{
		changingFogDensity(m_bFog);
	}
}

void DynamicFog::toggleFog()
{
	m_bFogFading = true;
	if (m_bFog)
	{
		m_bFog = false;
	}
	else
	{
		glEnable(GL_FOG);
		m_bFog = true;
	}
}

void DynamicFog::changingFogDensity(bool on)
{
	if (on && m_fFogDensity <= m_fFogMaxDensity)
	{
		m_fFogDensity += m_fFogDynamicSpeed * m_pCamera->getElapsedTime();
		glFogf(GL_FOG_DENSITY, m_fFogDensity);
	}
	else if (on && m_fFogDensity > m_fFogMaxDensity)
	{
		m_bFogFading = false;
	}
	else if (!on && m_fFogDensity >= 0.0f)
	{
		m_fFogDensity -= m_fFogDynamicSpeed * m_pCamera->getElapsedTime();
		glFogf(GL_FOG_DENSITY, m_fFogDensity);
	}
	else if (!on && m_fFogDensity < 0.0f)
	{
		m_bFogFading = false;
		glDisable(GL_FOG);
	}
}
