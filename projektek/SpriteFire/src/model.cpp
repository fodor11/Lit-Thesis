#include "../include/model.hpp"
#include <sstream>
#include <math.h>

vec3f::vec3f(void)
{
	m_x = 0.0f;
	m_y = 0.0f;
	m_z = 0.0f;
}

vec3f::vec3f(float x, float y, float z)
{
	m_x = x;
	m_y = y;
	m_z = z;
}

vec3f::vec3f(vec3f & vector)
{
	m_x = vector.x();
	m_y = vector.y();
	m_z = vector.z();
}

vec3f::vec3f(const vec3f & vector) 
{
	m_x = vector.x();
	m_y = vector.y();
	m_z = vector.z();
}

float vec3f::x() const
{
	return m_x;
}

float vec3f::y() const
{
	return m_y;
}

float vec3f::z() const
{
	return m_z;
}

void vec3f::setX(float value)
{
	m_x = value;
}

void vec3f::setY(float value)
{
	m_y = value;
}

void vec3f::setZ(float value)
{
	m_z = value;
}

vec3f::operator string() const
{
	stringstream ss;
	ss << "x: " << m_x << ", y: " << m_y << ", z: " << m_z;
	return ss.str();
}


vec3f vec3f::operator+(const vec3f& other)
{
	return vec3f(m_x + other.x(), m_y + other.y(), m_z + other.z());
}

vec3f& vec3f::operator+=(const vec3f & rhs)
{
	*this = *this + rhs;
	return *this;
}

vec3f vec3f::operator-(const vec3f& other)
{
	return vec3f(m_x - other.x(), m_y - other.y(), m_z - other.z());
}

vec3f& vec3f::operator-=(const vec3f & rhs)
{
	*this = *this - rhs;
	return *this;
}

vec3f & vec3f::operator=(const vec3f& other)
{
	m_x = other.x();
	m_y = other.y();
	m_z = other.z();
	return *this;
}

bool vec3f::operator==(const vec3f other)
{
	if (m_x==other.x() && m_y == other.y() && m_z == other.z() )
	{
		return true;
	}
	else
	{
		return false;
	}
}

vec3f vec3f::operator*(const vec3f& other)
{
	return vec3f(m_y*other.z() - m_z*other.y(),
				m_z*other.x() - m_x*other.z(),
				m_x*other.y() - m_y*other.x());
}

vec3f & vec3f::operator*=(const vec3f & rhs)
{
	*this = *this * rhs;
	return *this;
}

vec3f vec3f::operator*(const float& value)
{
	return vec3f(m_x*value, m_y*value, m_z*value);
}

vec3f & vec3f::operator*=(const float & rhs)
{
	*this = *this * rhs;
	return *this;
}

vec3f vec3f::operator/(const float & value)
{
	return vec3f(m_x/value, m_y/value, m_z/value);
}

vec3f & vec3f::operator/=(const float & rhs)
{
	*this = *this / rhs;
	return *this;
}

float & vec3f::operator[](int index) 
{
	switch (index)
	{
		case 0:
			return m_x;
			break;
		case 1:
			return m_y;
			break;
		case 2:
			return m_z;
			break;
	}
}

vec3f vec3f::multiplicate(vec3f & other)
{
	return vec3f(m_x*other.x(), m_y*other.y(), m_z*other.z());
}



void vec3f::normalize()
{
	float curr_length = length();
	m_x /= curr_length;
	m_y /= curr_length;
	m_z /= curr_length;
}

float vec3f::length() const
{
	return sqrt(powf(m_x, 2) + powf(m_y, 2) + powf(m_z, 2));
}

vec3f::~vec3f()
{
}

