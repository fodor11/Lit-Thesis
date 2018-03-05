#include "../include/texture.hpp"
#include "../include/heightmap.hpp"
#include <array>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

#ifdef __linux__
#include <cmath>
#endif

void HeightMapLoader::getHeightValues()
{
	m_pHeightValues = new float[m_width * m_height];

	for (int i = 0; i < m_width * m_height; i++)
	{
		m_pHeightValues[i] = pow((float)m_pImage[i] / 255.0f, 2);
		if (m_pHeightValues[i] > m_highestPoint)
		{
			m_highestPoint = m_pHeightValues[i];
		}
		else if (m_pHeightValues[i] < m_lowestPoint)
		{
			m_lowestPoint = m_pHeightValues[i];
		}
	}
}

void HeightMapLoader::determineColors()
{
	float lightness = 1.f;
	float maxShininess = 64;
	m_pColors = new vec3f[m_width * m_height];
	m_pShininess = new float[m_width * m_height];

	TextureLoader *biome = new TextureLoader();
	biome->loadImage("biome-smooth.png");

	for (int i = 0; i < m_width * m_height; i++)
	{
		float realtiveHeight = (m_pHeightValues[i] - m_lowestPoint) / (m_highestPoint - m_lowestPoint);
		float relativeMoisture = (m_pMoisture[i] - m_dryestPoint) / (m_wettestPoint - m_dryestPoint);
		m_pColors[i] = biome->getMoistureColor(relativeMoisture, realtiveHeight) * lightness;
		m_pShininess[i] = realtiveHeight * relativeMoisture * maxShininess;
	}
}

void HeightMapLoader::createVAO()
{
	TextureLoader texLoader;
	texLoader.loadMipMappedTexture("ground_soil.jpg");
	m_iTextureID = (GLuint) texLoader.textureID();
	texLoader.~TextureLoader();

	// create and bind the VAO
	glGenVertexArrays(1, &m_iTerrainVAO);
	glBindVertexArray(m_iTerrainVAO);

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> textureCoords;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> diffuseColors;
	std::vector<glm::vec3> ambientColors;
	std::vector<glm::vec3> specularColors;
	std::vector<float> shininesses;

	glm::vec3 vertex;
	glm::vec2 texture;
	glm::vec3 normal;
	glm::vec3 color;
	glm::vec3 specularColor = glm::vec3(1.0f, 1.0f, 1.0f);

	for (int i = 0; i < (m_width - 1); i++)
	{
		for (int j = 0; j < (m_height - 1); j++)
		{
			//vertex
			vertex = glm::vec3(i + 1, getHeight(i + 1, j), j);
			//txture u,v
			texture = glm::vec2(i, j);
			//color 
			color = getColor(i + 1, j);
			diffuseColors.push_back(color);
			color = color * 0.1f;
			ambientColors.push_back(color);
			color = color * 12.f;
			specularColors.push_back(color);
			shininesses.push_back(m_pShininess[j * m_width + i + 1]);
			//normal
			normal = getNormal(i + 1, j);

			vertices.push_back(vertex);
			textureCoords.push_back(texture);
			normals.push_back(normal);

			
			//vertex
			vertex = glm::vec3(i, getHeight(i, j), j);
			//txture u,v
			texture = glm::vec2(i, j + 1);
			//color 
			color = getColor(i, j);
			diffuseColors.push_back(color);
			color = color * 0.1f;
			ambientColors.push_back(color);
			color = color * 12.f;
			specularColors.push_back(color);
			shininesses.push_back(m_pShininess[j * m_width + i]);
			//normal
			normal = getNormal(i, j);

			vertices.push_back(vertex);
			textureCoords.push_back(texture);
			normals.push_back(normal);

			
			//vertex
			vertex = glm::vec3(i + 1, getHeight(i + 1, j + 1), j + 1);
			//txture u,v
			texture = glm::vec2(i + 1, j);
			//color 
			color = getColor(i + 1, j + 1);
			diffuseColors.push_back(color);
			color = color * 0.1f;
			ambientColors.push_back(color);
			color = color * 12.f;
			specularColors.push_back(color);
			shininesses.push_back(m_pShininess[(j + 1) * m_width + i + 1]);
			//normal
			normal = getNormal(i + 1, j + 1);

			vertices.push_back(vertex);
			textureCoords.push_back(texture);
			normals.push_back(normal);

			
			//vertex
			vertex = glm::vec3(i, getHeight(i, j + 1), j + 1);
			//txture u,v
			texture = glm::vec2(i + 1, j + 1);
			//color 
			color = getColor(i, j + 1);
			diffuseColors.push_back(color);
			color = color * 0.1f;
			ambientColors.push_back(color);
			color = color * 12.f;
			specularColors.push_back(color);
			shininesses.push_back(m_pShininess[(j + 1) * m_width + i]);
			//normal
			normal = getNormal(i, j + 1);

			vertices.push_back(vertex);
			textureCoords.push_back(texture);
			normals.push_back(normal);
		}
	}
	// create and bind the VBO for vertices
	glGenBuffers(1, &m_iTerrainVBO);
	m_vVBOs.push_back(m_iTerrainVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_iTerrainVBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices.front(), GL_STATIC_DRAW);
	// connect the xyz to the "vert" attribute of the vertex shader
	glEnableVertexAttribArray(m_pProgram->attrib("vert"));
	glVertexAttribPointer(m_pProgram->attrib("vert"), 3, GL_FLOAT, GL_FALSE, 0, NULL);

	// create and bind the VBO for texture coordinates
	glGenBuffers(1, &m_iTerrainVBO);
	m_vVBOs.push_back(m_iTerrainVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_iTerrainVBO);
	glBufferData(GL_ARRAY_BUFFER, textureCoords.size() * sizeof(glm::vec2), &textureCoords.front(), GL_STATIC_DRAW);
	// connect the uv coords to the "vertTexCoord" attribute of the vertex shader
	glEnableVertexAttribArray(m_pProgram->attrib("vertTexCoord"));
	glVertexAttribPointer(m_pProgram->attrib("vertTexCoord"), 2, GL_FLOAT, GL_FALSE, 0, NULL);
	
	// create and bind the VBO for ambientColors
	glGenBuffers(1, &m_iTerrainVBO);
	m_vVBOs.push_back(m_iTerrainVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_iTerrainVBO);
	glBufferData(GL_ARRAY_BUFFER, ambientColors.size() *  sizeof(glm::vec3), &ambientColors.front(), GL_STATIC_DRAW);
	// connect the rgb to the "vertAmbientColor" attribute of the vertex shader
	glEnableVertexAttribArray(m_pProgram->attrib("vertAmbientColor"));
	glVertexAttribPointer(m_pProgram->attrib("vertAmbientColor"), 3, GL_FLOAT, GL_TRUE, 0, NULL);

	// create and bind the VBO for diffuseColors
	glGenBuffers(1, &m_iTerrainVBO);
	m_vVBOs.push_back(m_iTerrainVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_iTerrainVBO);
	glBufferData(GL_ARRAY_BUFFER, diffuseColors.size() *  sizeof(glm::vec3), &diffuseColors.front(), GL_STATIC_DRAW);
	// connect the rgb to the "vertDiffuseColor" attribute of the vertex shader
	glEnableVertexAttribArray(m_pProgram->attrib("vertDiffuseColor"));
	glVertexAttribPointer(m_pProgram->attrib("vertDiffuseColor"), 3, GL_FLOAT, GL_TRUE, 0, NULL);

	// create and bind the VBO for specularColors
	glGenBuffers(1, &m_iTerrainVBO);
	m_vVBOs.push_back(m_iTerrainVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_iTerrainVBO);
	glBufferData(GL_ARRAY_BUFFER, specularColors.size() *  sizeof(glm::vec3), &specularColors.front(), GL_STATIC_DRAW);
	// connect the rgb to the "vertSpecularColor" attribute of the vertex shader
	glEnableVertexAttribArray(m_pProgram->attrib("vertSpecularColor"));
	glVertexAttribPointer(m_pProgram->attrib("vertSpecularColor"), 3, GL_FLOAT, GL_TRUE, 0, NULL);

	// create and bind the VBO for shininess
	glGenBuffers(1, &m_iTerrainVBO);
	m_vVBOs.push_back(m_iTerrainVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_iTerrainVBO);
	glBufferData(GL_ARRAY_BUFFER, shininesses.size() *  sizeof(float), &shininesses.front(), GL_STATIC_DRAW);
	// connect the rgb to the "vertSpecularColor" attribute of the vertex shader
	glEnableVertexAttribArray(m_pProgram->attrib("vertShininess"));
	glVertexAttribPointer(m_pProgram->attrib("vertShininess"), 1, GL_FLOAT, GL_FALSE, 0, NULL);


	// create and bind the VBO for normals
	glGenBuffers(1, &m_iTerrainVBO);
	m_vVBOs.push_back(m_iTerrainVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_iTerrainVBO);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals.front(), GL_STATIC_DRAW);
	// connect the normal to the "vertNorm" attribute of the vertex shader
	glEnableVertexAttribArray(m_pProgram->attrib("vertNorm"));
	glVertexAttribPointer(m_pProgram->attrib("vertNorm"), 3, GL_FLOAT, GL_TRUE, 0, NULL);
	
	std::vector<std::array<GLuint, 3>> indices;
	std::array<GLuint, 3> face;
	for (int i = 0; i < (m_width-2); i++)
	{
		for (int j = 0; j < (m_height-2); j++)
		{
			// drawing 2 triangles ()
			int starterValue = (i * (m_height - 2) + j) * 4;			
			face[0] = starterValue;
			face[1] = starterValue + 1;
			face[2] = starterValue + 2;
			indices.push_back(face);
			
			starterValue = starterValue + 2;
			face[0] = starterValue;
			face[1] = starterValue - 1;
			face[2] = starterValue + 1;
			indices.push_back(face);
		}
	}
	// create the IBO
	m_iNumberOfIndicies = indices.size() * 3;
	glGenBuffers(1, &m_iIndexVBO);
	m_vVBOs.push_back(m_iIndexVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iIndexVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_iNumberOfIndicies * sizeof(GLuint), &indices.front(), GL_STATIC_DRAW);
	
	// unbind the VAO
	glBindVertexArray(0);
}

void HeightMapLoader::getMoistValues()
{
	m_fileName = "moisture.png";
	LoadingGreyScaleImage();
	m_pMoisture = new float[m_width*m_height];
	for (int i = 0; i < m_width*m_height; i++)
	{
		m_pMoisture[i] = (float)m_pImage[i] / 255.0f;
		if (m_pMoisture[i] > m_wettestPoint)
		{
			m_wettestPoint = m_pMoisture[i];
		}
		else if (m_pMoisture[i] < m_dryestPoint)
		{
			m_dryestPoint = m_pMoisture[i];
		}
	}
}

void HeightMapLoader::LoadingGreyScaleImage()
{
	m_pImage = SOIL_load_image(m_fileName.c_str(), &m_width, &m_height, &m_channels, SOIL_LOAD_L);
	if (m_pImage == nullptr)
	{
		std::cout << m_fileName.c_str() << " could not be loaded!" << std::endl;
	}
	else
	{
		std::cout << m_fileName.c_str() << " loaded! resolution: " << m_width << "x" << m_height << std::endl;
	}
}

bool HeightMapLoader::existingCoord(int x, int z)
{
	if (x >= 0 && z >= 0 && x < m_width && z < m_height)
	{
		return true;
	}
	else
	{
		return false;
	}
}
vec3f calcNormalFromPoints(vec3f& p0, vec3f& p1, vec3f& p2)
{
	vec3f v1 = p1 - p0;
	vec3f v2 = p2 - p0;
	return v1*v2;
}
void HeightMapLoader::calcVertexNormals()
{
	std::cout << "Generating normals...\t\t";
	m_pVertexNormals = new vec3f[m_width*m_height];
	vec3f p0(0, 0, 0);
	vec3f p1(0, 0, 0);
	vec3f p2(0, 0, 0);
	int triangles;
	for (int i = 0; i < m_width; i++)
	{
		for (int j = 0; j < m_height; j++)
		{
			vec3f& actualNormal = m_pVertexNormals[i*m_width + j];
			actualNormal = vec3f(0, 0, 0);
			p0 = vec3f(i, getUnitHeight(i, j), j);
			triangles = 0;
			//second
			if (existingCoord(i - 1, j))
			{
				if (existingCoord(i - 1, j + 1))
				{
					p1 = vec3f(i - 1, getUnitHeight(i - 1, j), j);
					p2 = vec3f(i - 1, getUnitHeight(i - 1, j + 1), j + 1);
					triangles++;
					actualNormal += calcNormalFromPoints(p0, p1, p2);
				}
			}
			//third - if 3rd exists, then 4th exists too
			if (existingCoord(i - 1, j + 1))
			{
				p1 = vec3f(i - 1, getUnitHeight(i - 1, j + 1), j + 1);
				p2 = vec3f(i, getUnitHeight(i, j + 1), j + 1);
				triangles++;
				actualNormal += calcNormalFromPoints(p0, p1, p2);
				//check if 5th exists => p1=4th, p2=5th
				if (existingCoord(i + 1, j))
				{
					p1 = vec3f(i, getUnitHeight(i, j + 1), j + 1);
					p2 = vec3f(i + 1, getUnitHeight(i + 1, j), j);
					triangles++;
					actualNormal += calcNormalFromPoints(p0, p1, p2);
				}
			}
			//fifth
			if (existingCoord(i + 1, j))
			{
				if (existingCoord(i + 1, j - 1))
				{
					p1 = vec3f(i + 1, getUnitHeight(i + 1, j), j);
					p2 = vec3f(i + 1, getUnitHeight(i + 1, j - 1), j - 1);
					triangles++;
					actualNormal += calcNormalFromPoints(p0, p1, p2);
				}
			}
			//sixth => if 6th exists, then 1st exists too
			if (existingCoord(i + 1, j - 1))
			{
				p1 = vec3f(i + 1, getUnitHeight(i + 1, j - 1), j - 1);
				p2 = vec3f(i, getUnitHeight(i, j - 1), j - 1);
				triangles++;
				actualNormal += calcNormalFromPoints(p0, p1, p2);
				//check if 2nd exists => p1=1st p2=2nd
				if (existingCoord(i - 1, j))
				{
					p1 = vec3f(i, getUnitHeight(i, j - 1), j - 1);
					p2 = vec3f(i - 1, getUnitHeight(i - 1, j), j);
					triangles++;
					actualNormal += calcNormalFromPoints(p0, p1, p2);
				}
			}

			actualNormal /= triangles;
			actualNormal.normalize();
		}
	}
	std::cout << "Done\n";
}

HeightMapLoader::HeightMapLoader(std::string fileName, tdogl::Program* program)
{
	m_pProgram = program;
	//moisture
	getMoistValues();
	SOIL_free_image_data(m_pImage);
	//height
	m_fileName = fileName;
	LoadingGreyScaleImage();
	getHeightValues();
	calcVertexNormals();
	SOIL_free_image_data(m_pImage);
	//color
	determineColors();
	//VAO
	createVAO();
}

void HeightMapLoader::drawTerrain() const
{
	// bind the program (the shaders)
	m_pProgram->use();

	// set the "model" uniform in the vertex shader
	m_pProgram->setUniform("model", m_mModel);

	// set the "normalMatrix" uniform in the vertex shader
	m_pProgram->setUniform("normalMatrix", glm::mat3(glm::transpose(glm::inverse(m_mModel))));

	// bind the texture and set the "tex" uniform in the fragment shader
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_iTextureID);
	m_pProgram->setUniform("tex", 0); //set to 0 because the texture is bound to GL_TEXTURE0

	// bind the VAO 
	glBindVertexArray(m_iTerrainVAO);
	// draw the IBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iIndexVBO);
	glDrawElements(GL_TRIANGLES, m_iNumberOfIndicies, GL_UNSIGNED_INT, NULL);
	// unbind the IBO, VAO, program and the texture
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	m_pProgram->stopUsing();
}

HeightMapLoader::~HeightMapLoader()
{
	delete[]  m_pVertexNormals;
	delete  m_pHeightValues;
	delete m_pMoisture;
	delete[] m_pColors;
	delete[] m_pShininess;
	glDeleteBuffers(m_vVBOs.size(), &m_vVBOs.front());
	std::cout << "heightmap destroyed" << std::endl;
}

int HeightMapLoader::getImageWidth() const
{
	return m_width;
}

int HeightMapLoader::getImageHeight() const
{
	return m_height;
}

float HeightMapLoader::linearInterpolation(float pointA, float pointB, float weightB)
{
	float weightA = 1 - weightB;
	float ret = pointA * weightA + pointB * weightB;
	return ret;
}
float HeightMapLoader::getUnitHeight(float x, float z)
{
	if (x < (m_width - 1) && z < (m_height - 1) && x >= 0 && z >= 0)
	{
		float xf1, zf1;
		float weight1 = modff(x, &xf1);
		float weight2 = modff(z, &zf1);
		int x1 = (int)xf1;
		int z1 = (int)zf1;
		float lin1 = linearInterpolation(m_pHeightValues[z1 * m_width + x1], m_pHeightValues[z1 * m_width + (x1 + 1)], weight1);
		float lin2 = linearInterpolation(m_pHeightValues[(z1 + 1) * m_width + x1], m_pHeightValues[(z1 + 1) * m_width + (x1 + 1)], weight1);
		float ret = linearInterpolation(lin1, lin2, weight2);
		return ret;
	}
	else if (x == (m_width - 1) || z == (m_height - 1))
	{
		return m_pHeightValues[(int)z * m_width + (int)x];
	}
	else
	{
		return 0.0f;
	}
}

float HeightMapLoader::getHeight(float x, float z)
{
	return getUnitHeight(x, z) * m_maxHeight;
}

glm::vec3 HeightMapLoader::getNormal(int x, int z) const
{
	glm::vec3 retNormal = glm::vec3(0.f, 0.f, 0.f);
	if (x < m_width && z < m_height && x >= 0 && z >= 0)
	{
		vec3f tmpNorm = m_pVertexNormals[x*m_width + z];
		retNormal = glm::vec3(tmpNorm.x(), tmpNorm.y(), tmpNorm.z());
	}
	return retNormal;
}

glm::vec3 HeightMapLoader::getColor(int x, int z) const
{
	vec3f tmpColor = m_pColors[z*m_width + x];
	glm::vec3 retColor(tmpColor.x(), tmpColor.y(), tmpColor.z());
	return retColor;
}

float HeightMapLoader::getScale() const
{
	return m_scale;
}

float HeightMapLoader::getMaxHeight() const
{
	return m_maxHeight;
}

