#pragma once

#include <assimp/vector3.h>

enum class EAxis
{
	PositiveX,
	NegativeX,
	PositiveY,
	NegativeY,
	PositiveZ,
	NegativeZ,
};

inline aiVector3D Vec3ConvertUp(const aiVector3D& _v, EAxis _up)
{
	aiVector3D res;
	if (_up == EAxis::NegativeY)
	{
		res.x = _v.x;
		res.y = -_v.y;
		res.z = -_v.z;
	}
	else //if (_up == EAxis::PositiveZ)
	{
		res.x = _v.x;
		res.y = _v.z;
		res.z = _v.y;
	}
	return res;
}

inline aiVector3D Vec3Cross(const aiVector3D& _v1, const aiVector3D& _v2)
{
	aiVector3D res;
	res.x = _v1.y * _v2.z - _v1.z * _v2.y;
	res.y = _v1.z * _v2.x - _v1.x * _v2.z;
	res.z = _v1.x * _v2.y - _v1.y * _v2.x;
	return res;
}

inline float Vec3Dot(const aiVector3D& _v1, const aiVector3D& _v2)
{
	return (_v1.x * _v2.x
		+ _v1.y * _v2.y
		+ _v1.z * _v2.z);
}

inline float GetBitangentSign(
	const aiVector3D& _normal,
	const aiVector3D& _tangent,
	const aiVector3D& _bitangent)
{
	aiVector3D cross = Vec3Cross(_normal, _tangent);
	float dot = Vec3Dot(cross, _bitangent);
	return (dot < 0.0f) ? -1.0f : 1.0f;
}
