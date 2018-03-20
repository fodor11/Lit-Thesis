#pragma once
#include <iostream>
#include "texture.hpp"

using namespace std;

class vec3f
{
public:
	/// Constructors
	vec3f(void);
	vec3f(float x, float y, float z);
	vec3f(vec3f& vector);
	vec3f(const vec3f& vector);

	/// get values
	float x() const;
	float y() const;
	float z() const;

	/// set values
	void setX(float value);
	void setY(float value);
	void setZ(float value);

	/// string cast
	operator string() const;

	/// sum of an another vector and this one
	vec3f operator+(const vec3f& other);
	vec3f& operator+=(const vec3f& rhs);

	/// difference of an another vector and this one
	vec3f operator-(const vec3f& other);
	vec3f& operator-=(const vec3f& rhs);

	/// = operator - copy
	vec3f& operator=(const vec3f& other);

	/// == equals operator
	bool operator==(const vec3f other);

	/// cross product
	vec3f operator*(const vec3f& other);
	vec3f& operator*=(const vec3f& rhs);

	/// multiplying with number
	vec3f operator*(const float& value);
	vec3f& operator*=(const float& rhs);

	/// dividing by number
	vec3f operator/(const float& value);
	vec3f& operator/=(const float& rhs);

	/// get x,y,z with []
	float& operator[](int index);

	/// multiplicating by members
	vec3f multiplicate(vec3f& other);

	/// normalizing
	void normalize();
	/// length of the vector
	float length() const;

	~vec3f();

private:
	float m_x;
	float m_y;
	float m_z;
};

