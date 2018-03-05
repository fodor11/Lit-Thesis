#pragma once
#include <gl/glew.h>
#include <gl/freeglut.h>

#include <vector>
#include "model.hpp"
#include "camera.hpp"

class RainDrop
{
	friend class Rain;
public:
	/// sets the position
	RainDrop(vec3f& position);
	/// empty
	~RainDrop();
	/// draws the line considering the given direction
	void draw(vec3f& direction);
private:
	vec3f m_position;
	float m_fDropLength = 0.5f;
	bool m_bVisible = true;
};

class Rain
{
public:
	/// fills m_vRaindrops and
	/// randomizes the position of the drops within the range (within a cube)
	Rain(Camera* camera, HeightMapLoader* heightmap);
	/// empty
	~Rain();
	/// moves the drops and checks if the drop is still in the box
	/// if not, then replaces it to fit in the box
	void update();
	/// changes direction by the given vector values
	void changeDirectionBy(vec3f& values);
	/// toggles raining on and off
	void toggleRain();
private:
	/// defines if it is raining or not
	bool m_bRaining = false;
	bool m_bStopRaining = false;
	int m_iNumOfDrops = 500;
	float m_fRainSpeed = 0.5f;
	float m_fDropLength = 0.5f;
	vec3f m_direction = vec3f(0.f, 1.0f, 0.f) * m_fDropLength;
	
	/// side of the range square will be 2 x m_fRainSquareSize 
	///    |
	/// -- @ -- like this
	///    |
	float m_fRainSquareSize = 15.0f;
	float m_fRainMaxHeight = 2 * m_fRainSquareSize;

	void checkPosition(RainDrop& drop);
	int m_iStoppedDrops = 0;
	void stopRain();
	/// rearranges drops, so that they are higher above the ground and more distributed,
	/// making the next rain starting smoother
	void rearrangePositions();

	std::vector<RainDrop> m_vRaindrops;
	Camera* m_pCamera;
	HeightMapLoader* m_pHeightmap;
};
