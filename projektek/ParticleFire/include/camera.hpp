#pragma once

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif

#include <glm/glm.hpp>
#include "heightmap.hpp"
#include "model.hpp"
#include <math.h>
#include <cmath>
#include <iostream>
#include <map>
#include <vector>
#include "../src/tdogl/Program.h"

#ifdef __linux__
#define LARGE_INTEGER long
#else
#include <Windows.h>
#endif

class Camera
{
public:
	/// starts timer, gets heightmap, sets the middle of the map
	Camera(HeightMapLoader* heightMap, std::vector<tdogl::Program*> shaderPrograms);
	/// empty
	~Camera();
	/// moves the camera
	void updateCamera();
	/// sets the m_bMoveForward (to true if false, to false if true)
	void setForwardMovement();
	/// sets the m_bMoveBackward (to true if false, to false if true)
	void setBackwardMovement();
	/// sets the m_bMoveLeft (to true if false, to false if true)
	void setLeftMovement();
	/// sets the m_bMoveRight (to true if false, to false if true)
	void setRightMovement();
	/// sets the m_bMoveRight (to true if false, to false if true)
	void setUpwardMovement();
	/// sets the m_bMoveRight (to true if false, to false if true)
	void setDownwardMovement();
	/// adds the rotation on the XZ plane to the current rotation
	void addRotationInRadian(float addRotationAngle);
	/// adds the horizontal angle to the current horizontal angle
	void changeHorizonInRadian(float addHorizonAngle);
	/// returns the X coordinate of the camera position
	float getX() const;
	/// returns the Y coordinate of the camera position
	float getY() const;
	/// returns the Z coordinate of the camera position
	float getZ() const;
	/// returns the position of the camera
	glm::vec3 getPosition();
	/// returns the height of the camera
	float getCameraHeight() const;
	/// returns elapsed time
	float getElapsedTime() const;
	/// sets the list of obstacles' positions
	void setObstacles(std::map<std::string, std::vector<vec3f>>* obstaclePositions);
	///starts the timer
	void startTimer();
	/// sets aspect ratio
	void setAspectRatio(float ratio);
	/// get projection matrix
	glm::mat4 getProjectionMatrix();
	/// get view matrix
	glm::mat4 getViewMatrix();
private:
	/// projection
	float m_fFieldOfView = 50.0f;
	float m_fNearPlane = 0.1f;
	float m_fFarPlane = 650.f;
	float m_fViewportAspectRatio = 4.f/3.f;
	/// matrices
	glm::mat4 m_mView;
	glm::mat4 m_mProjection;
	/// shader program
	std::vector<tdogl::Program*> m_pPrograms;
	/// rotation in the Y axis
	float m_fRotationAngleRadian = 0.f;
	/// horizon angle in radian
	float m_fHorizonAngleRadian = 0.0f;

	/// position of the camera
	float m_fCameraX = 0.0f, 
		  m_fCameraY = 1.0f, 
		  m_fCameraZ = 0.0f;
	/// "height" of the camera
	float m_fCameraHeight = 1.f;
	/// camera's direction (vector)
	float m_fDirectionX = 0.0f,
		  m_fDirectionZ = 0.0f;

	/// previous positions (used in collision detection)
	float m_fPrevCameraX = 0.0f,
		m_fPrevCameraZ = 0.0f;
	/// unavailable positions
	std::map<std::string, std::vector<vec3f>>* m_pObstaclePositions = nullptr;
	/// radius of the obstacles
	float m_fObstacleRadius = 0.15f;
	/// returns true if the current position is inside an obstacle's radius
	bool checkCollisions();
	/// calculates distance between 2 2D points
	float calc2Ddistance(float point1x, float point1y, float point2x, float point2y);

	/// speed of camera
	float m_fSpeed = 0.005f;

	/// movements
	bool m_bMoveForward = false,
		 m_bMoveBackward = false,
		 m_bMoveLeft = false,
		 m_bMoveRight = false,
		 m_bMoveUp = false,
		 m_bMoveDown = false;

	HeightMapLoader* m_pHeightMap;

	void move();
	float radianToAngle(float radian);

	LARGE_INTEGER m_liPrevTime, m_liCurrTime, m_liFrequency;
	float m_fElapsedTime = 0.0f;
	float calcElapsedTime();
};
