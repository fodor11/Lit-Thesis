#include "../include/texture.hpp"
#include <iostream>

/*
* Texture loader
*/
void TextureLoader::loadTexture(std::string filename)
{
	m_fileName = filename;
	m_textureId = 0;
	m_textureId = SOIL_load_OGL_texture(m_fileName.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	if (m_textureId == 0)
	{
		std::cout << m_fileName.c_str() << " could not be loaded!" << std::endl;
	}
	else
		std::cout << m_fileName.c_str() << " loaded!" << std::endl;

	glBindTexture(GL_TEXTURE_2D, m_textureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //when the shown picture is streched
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //when the shown picture is smaller
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //coordinate outside the range of 0 to 1 (s,t = x,y)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void TextureLoader::loadImage(std::string filename)
{
	m_fileName = filename;
	m_pixel = (Pixel*)SOIL_load_image(m_fileName.c_str(), &m_width, &m_height, 0, SOIL_LOAD_RGB);
	if (m_pixel == nullptr)
	{
		std::cout << m_fileName.c_str() << " could not be loaded!" << std::endl;
	}
	else
		std::cout << m_fileName.c_str() << " loaded!" << std::endl;
}

vec3f TextureLoader::getMoistureColor(float moisture, float height)
{
	if (m_pixel != nullptr && moisture >= 0 && height >= 0 && moisture <= 1.00001 && height <= 1.00001)
	{
		int x = m_width*moisture;
		int y = m_height*(1 - height);
		int index = y*m_width + x;
		float r = m_pixel[index][0];
		float g = m_pixel[index][1];
		float b = m_pixel[index][2];
		return vec3f(r / 255, g / 255, b / 255);
	}
	else return vec3f(1, 0, 0);
}

vec3f TextureLoader::getPixelColor(int x, int y)
{
	if (m_pixel != nullptr && x >= 0 && y >= 0 && x < m_width && y < m_height)
	{
		y = (m_height - 1) - y;
		int index = y*m_width + x;
		float r = m_pixel[index][0];
		float g = m_pixel[index][1];
		float b = m_pixel[index][2];
		return vec3f(r / 255, g / 255, b / 255);
	}
	else return vec3f(0, 0, 0);
}

int TextureLoader::getWidth() const
{
	return m_width;
}

int TextureLoader::getHeight() const
{
	return m_height;
}

unsigned int TextureLoader::textureID() const
{
	return m_textureId;
}

TextureLoader::TextureLoader() {}

TextureLoader::~TextureLoader() {}

void TextureLoader::loadMipMappedTexture(std::string filename)
{
	m_fileName = filename;

	glGenTextures(1, &m_textureId);
	glBindTexture(GL_TEXTURE_2D, m_textureId);
	glEnable(GL_TEXTURE_2D);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	unsigned char* imageData;
	imageData = SOIL_load_image(m_fileName.c_str(), &m_width, &m_height, 0, SOIL_LOAD_RGBA);
	if (imageData == nullptr)
	{
		std::cout << m_fileName.c_str() << " could not be loaded!" << std::endl;
	}
	else
		std::cout << m_fileName.c_str() << " loaded!" << std::endl;

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //when the shown picture is streched
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); //when the shown picture is smaller
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //coordinate outside the range of 0 to 1 (s,t = x,y)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//gluBuild2DMipmaps(GL_TEXTURE_2D, 4, m_width, m_height, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
	
	SOIL_free_image_data(imageData);
}
