#pragma once
#include <math.h>

class Vector {
public:
	Vector() {}
	Vector(float _X, float _Y, float _Z) : x(_X), y(_Y), z(_Z) {}
	Vector(float _X, float _Y) : x(_X), y(_Y), z(0) {}

	float x = 0;
	float y = 0;
	float z = 0;

	float distanceTo(Vector const other) {
		Vector dist;
		dist.x = other.x - this->x;
		dist.y = other.y - this->y;
		dist.z = other.z - this->z;
		return sqrt(dist.x * dist.x + dist.y * dist.y + dist.z * dist.z);
	}

	Vector operator-(Vector& r)
	{
		return Vector(x - r.x, y - r.y, z - r.z);
	}

	float length()
	{
		return sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
	}

};
