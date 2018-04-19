#include "../include/camera.hpp"
#include <glm/gtc/matrix_transform.hpp>
Camera::Camera(HeightMapLoader* heightMap, std::vector<tdogl::Program*> shaderPrograms)
{
#ifndef __linux__
	QueryPerformanceFrequency(&m_liFrequency);
#endif
	m_pPrograms = shaderPrograms;
	m_pHeightMap = heightMap;
	m_fCameraX = (heightMap->getImageWidth()*heightMap->getScale()) / 2;
	m_fCameraZ = (heightMap->getImageHeight()*heightMap->getScale()) / 2;
}


Camera::~Camera(){}

void Camera::updateCamera()
{
	move();

	m_mProjection = glm::perspective(glm::radians(m_fFieldOfView), m_fViewportAspectRatio, m_fNearPlane, m_fFarPlane);
	m_mView = glm::rotate(glm::mat4(), m_fHorizonAngleRadian, glm::vec3(1, 0, 0));
	// camera had to be rotated by 90 degrees to match the directions of m_fDirectionX and m_fDirectionZ
	m_mView = glm::rotate(m_mView, (float)(m_fRotationAngleRadian + M_PI_2), glm::vec3(0, 1, 0));
	m_mView = glm::translate(m_mView, glm::vec3(-m_fCameraX, -m_fCameraY, -m_fCameraZ));
	//std::cout << "angle: " << glm::degrees(m_fRotationAngleRadian + M_PI_2) << "x,z: " << m_fCameraX<<", " <<m_fCameraZ << std::endl;

	glm::mat4 camera = m_mProjection * m_mView;
	//set the "camera" uniform in the vertex shaders
	for each (tdogl::Program* shader in m_pPrograms)
	{
		shader->use();
		shader->setUniform("camera", camera);
		shader->setUniform("viewPosition", glm::vec3(m_fCameraX, m_fCameraY, m_fCameraZ));
		shader->stopUsing();
	}
}

void Camera::setForwardMovement()
{
	m_bMoveForward = !m_bMoveForward;
}

void Camera::setBackwardMovement()
{
	m_bMoveBackward = !m_bMoveBackward;
}

void Camera::setLeftMovement()
{
	m_bMoveLeft = !m_bMoveLeft;
}

void Camera::setRightMovement()
{
	m_bMoveRight = !m_bMoveRight;
}

void Camera::setUpwardMovement()
{
	m_bMoveUp = !m_bMoveUp;
}

void Camera::setDownwardMovement()
{
	m_bMoveDown = !m_bMoveDown;
}

void Camera::addRotationInRadian(float addRotationAngle)
{
	m_fRotationAngleRadian += addRotationAngle;
	if (m_fRotationAngleRadian > (M_PI * 2) || m_fRotationAngleRadian < -(M_PI * 2))
	{
		m_fRotationAngleRadian = fmodf(m_fRotationAngleRadian, M_PI * 2);
	}
	m_fDirectionX = cos(m_fRotationAngleRadian);
	m_fDirectionZ = sin(m_fRotationAngleRadian);
}

void Camera::changeHorizonInRadian(float addHorizonAngle)
{
	m_fHorizonAngleRadian += addHorizonAngle;
	if (m_fHorizonAngleRadian < -M_PI_2)
	{
		m_fHorizonAngleRadian = -M_PI_2;
	}
	else if (m_fHorizonAngleRadian > M_PI_2)
	{
		m_fHorizonAngleRadian = M_PI_2;
	}
}

float Camera::getX() const
{
	return m_fCameraX;
}

float Camera::getY() const
{
	return m_fCameraY;
}

float Camera::getZ() const
{
	return m_fCameraZ;
}

glm::vec3 Camera::getPosition()
{
	return glm::vec3(m_fCameraX, m_fCameraY, m_fCameraZ);
}

float Camera::getCameraHeight() const
{
	return m_fCameraHeight;
}

float Camera::getElapsedTime() const
{
	return m_fElapsedTime;
}

void Camera::setObstacles(std::map<std::string, std::vector<vec3f>>* obstaclePositions)
{
	m_pObstaclePositions = obstaclePositions;
}

void Camera::startTimer()
{
#ifndef __linux__
	QueryPerformanceCounter(&m_liCurrTime);
#endif
}

void Camera::setAspectRatio(float ratio)
{
	m_fViewportAspectRatio = ratio;
}

glm::mat4 Camera::getProjectionMatrix()
{
	return m_mProjection;
}

glm::mat4 Camera::getViewMatrix()
{
	return m_mView;
}

void Camera::move()
{
	//float prevTime = m_fElapsedTime;
	//// smoothes movement in case of unstable fps
	//m_fElapsedTime = (prevTime + calcElapsedTime()) * 0.5f;
	m_fElapsedTime = calcElapsedTime();
	m_fPrevCameraX = m_fCameraX;
	m_fPrevCameraZ = m_fCameraZ;
	float distance = m_fSpeed * m_fElapsedTime;

	if (m_bMoveForward)
	{
		m_fCameraX += m_fDirectionX * distance;
		m_fCameraZ += m_fDirectionZ * distance;
	}
	if (m_bMoveBackward)
	{
		m_fCameraX -= m_fDirectionX * distance;
		m_fCameraZ -= m_fDirectionZ * distance;
	}
	if (m_bMoveRight)
	{
		m_fCameraX += cos(m_fRotationAngleRadian + M_PI_2) * distance;
		m_fCameraZ += sin(m_fRotationAngleRadian + M_PI_2) * distance;
	}
	if (m_bMoveLeft)
	{
		m_fCameraX -= cos(m_fRotationAngleRadian + M_PI_2) * distance;
		m_fCameraZ -= sin(m_fRotationAngleRadian + M_PI_2) * distance;
	}
	if (m_bMoveUp)
	{
		m_fCameraHeight += distance;
	}
	if (m_bMoveDown)
	{
		m_fCameraHeight -= distance;
	}
	if (checkCollisions())
	{
		m_fCameraX = m_fPrevCameraX;
		m_fCameraZ = m_fPrevCameraZ;
	}
	m_fCameraY = m_pHeightMap->getHeight(m_fCameraX, m_fCameraZ) + m_fCameraHeight;
}
float Camera::calc2Ddistance(float point1x, float point1y, float point2x, float point2y)
{
	return sqrtf( powf(point2x - point1x, 2.0f) + powf(point2y - point1y, 2.0f));
}
bool Camera::checkCollisions()
{
	if (m_pObstaclePositions != nullptr)
	{
		for (auto obstacle = m_pObstaclePositions->begin(); obstacle != m_pObstaclePositions->end(); obstacle++)
		{
			std::vector<vec3f>&  positions = obstacle->second;
			for (vec3f& position : positions)
			{
				if (calc2Ddistance(m_fCameraX, m_fCameraZ, position.x(), position.z()) < m_fObstacleRadius)
				{
					return true;
				}
			}
		}
	}
	return false;
}

float Camera::radianToAngle(float radian)
{
	return radian * (180 / M_PI);
}

float Camera::calcElapsedTime()
{
#ifndef __linux__
	m_liPrevTime = m_liCurrTime;
	QueryPerformanceCounter(&m_liCurrTime);
	float elapsed = (m_liCurrTime.QuadPart - m_liPrevTime.QuadPart) * 100.f / m_liFrequency.QuadPart;
#else
	static int prevTime = glutGet(GLUT_ELAPSED_TIME);
    int currentTime = glutGet(GLUT_ELAPSED_TIME);
    float elapsed = (float)(currentTime - prevTime) / 1000.0;
    prevTime = currentTime;
#endif
	if (elapsed < 0.01)
	{
		std::cout << "elapsed time < 0.01" << std::endl;
	}
	return elapsed;
}
