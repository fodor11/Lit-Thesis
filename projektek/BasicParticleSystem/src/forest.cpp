#include "../include/forest.hpp"
#include <iostream>
Forest::Forest(std::string fileName)
{
	m_sFilename = fileName;
}

Forest::~Forest()
{
}

void Forest::initialize(HeightMapLoader * heightmap)
{
	m_pHeightmap = heightmap;
	readPositions();
}

std::map<std::string, std::vector<vec3f>>* Forest::getPositionsMap()
{
	return &m_mPositions;
}

void Forest::readPositions()
{
	TextureLoader* population = new TextureLoader();
	population->loadImage(m_sFilename);
	int width = population->getWidth();
	int height = population->getHeight();
	float maxHeight = m_pHeightmap->getMaxHeight();
	vec3f tmpVector;

	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			tmpVector = population->getPixelColor(i, j);
			if (tmpVector == vec3f(1.f, 0.f, 0.f))
			{
				m_mPositions["tree1"].push_back(
							vec3f(i, m_pHeightmap->getUnitHeight(i, j) * maxHeight, j));
			}
			else if (tmpVector == vec3f(0.f, 1.f, 0.f))
			{
				m_mPositions["tree2"].push_back(
					vec3f(i, m_pHeightmap->getUnitHeight(i, j) * maxHeight, j));
			}
			else if (tmpVector == vec3f(0.f, 0.f, 1.f))
			{
				m_mPositions["tree3"].push_back(
					vec3f(i, m_pHeightmap->getUnitHeight(i, j) * maxHeight, j));
			}
		}
	}
	delete population;
}
