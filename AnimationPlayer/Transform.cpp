// author: Geonho Shin (icefin@pearlabyss.com)
#include "Transform.h"

void	copyMatrix4f(const mat4f src, mat4f dest)
{
	for (int32 r = 0; r < 4; r++)
	{
		for (int32 c = 0; c < 4; c++)
			dest[r][c] = src[r][c];
	}
}

void	multiplyMatrix4fToMatrix4f(const mat4f a, const mat4f b, mat4f dest)
{
	for (int32 r = 0; r < 4; r++)
	{
		for (int32 c = 0; c < 4; c++)
		{
			dest[r][c] = 0;
			for (int32 k = 0; k < 4; k++)
				dest[r][c] += (a[r][k] * b[k][c]);
		}
	}
}

void	multiplyMatrix4fToVector3f(const mat4f a, const vec3f& b, vec3f& dest)
{
	dest.x = (b.x * a[0][0]) + (b.y * a[0][1]) + (b.z * a[0][2]);
	dest.y = (b.x * a[1][0]) + (b.y * a[1][1]) + (b.z * a[1][2]);
	dest.z = (b.x * a[2][0]) + (b.y * a[2][1]) + (b.z * a[2][2]);
}


void	setIdentityMatrix4f(mat4f dest)
{
	for (int32 r = 0; r < 4; r++)
	{
		for (int32 c = 0; c < 4; c++)
			dest[r][c] = 0;
		dest[r][r] = 1;
	}
}

void	setRotationMatrix4fX(float radian, mat4f dest)
{
	dest[0][0] = 1;       dest[0][1] = 0;			 dest[0][2] = 0;			dest[0][3] = 0;
	dest[1][0] = 0;       dest[1][1] = cos(radian);  dest[1][2] = -sin(radian); dest[1][3] = 0;
	dest[2][0] = 0;       dest[2][1] = sin(radian);  dest[2][2] = cos(radian);  dest[2][3] = 0;
	dest[3][0] = 0;       dest[3][1] = 0;			 dest[3][2] = 0;			dest[3][3] = 1;
}

void	setRotationMatrix4fY(float radian, mat4f dest)
{
	dest[0][0] = cos(radian);	dest[0][1] = 0;		dest[0][2] = sin(radian);	 dest[0][3] = 0;
	dest[1][0] = 0;				dest[1][1] = 1;		dest[1][2] = 0;				 dest[1][3] = 0;
	dest[2][0] = -sin(radian);	dest[2][1] = 0;		dest[2][2] = cos(radian);	 dest[2][3] = 0;
	dest[3][0] = 0;				dest[3][1] = 0;		dest[3][2] = 0;				 dest[3][3] = 1;
}

void	setRotationMatrix4fZ(float radian, mat4f dest)
{
	dest[0][0] = cos(radian);	dest[0][1] = -sin(radian);	dest[0][2] = 0;		dest[0][3] = 0;
	dest[1][0] = sin(radian);	dest[1][1] = cos(radian);   dest[1][2] = 0;		dest[1][3] = 0;
	dest[2][0] = 0;				dest[2][1] = 0;				dest[2][2] = 1;		dest[2][3] = 0;
	dest[3][0] = 0;				dest[3][1] = 0;				dest[3][2] = 0;		dest[3][3] = 1;
}

float	magnitudeVector3f(const vec3f& u)
{
	return sqrtf((u.x * u.x) + (u.y * u.y) + (u.z * u.z));
}

float	dotProductVector3f(const vec3f& u, const vec3f& v)
{
	return ((u.x * v.x) + (u.y * v.y) + (u.z * v.z));
}

void	crossProductVector3f(const vec3f& u, const vec3f& v, vec3f& dest)
{
	dest.x = (u.y * v.z) - (u.z * v.y);
	dest.y = (u.z * v.x) - (u.x * v.z);
	dest.z = (u.x * v.y) - (u.y * v.x);
}

float	angleBetweenVector3f(const vec3f& u, const vec3f& v)
{
	float dot = dotProductVector3f(u, v);
	float magU = magnitudeVector3f(u);
	float magV = magnitudeVector3f(v);

	return (acos(dot / (magU * magV)));
}

float	degreeToRadian(float degree)
{
	return (degree * PI / 180.0);
}

float	radianToDegree(float radian)
{
	return (radian * 180.0 / PI);
}

Quaternion rotationMatrix4fToQuaternion(mat4f m)
{
	Quaternion quaternion;

	float trace = m[0][0] + m[1][1] + m[2][2];

	if (trace > 0)
	{
		float s = sqrtf(trace + 1.0f) * 2.0f;
		quaternion.w = 0.25f * s;
		quaternion.x = (m[2][1] - m[1][2]) / s;
		quaternion.y = (m[0][2] - m[2][0]) / s;
		quaternion.z = (m[1][0] - m[0][1]) / s;
	}
	else if ((m[0][0] > m[1][1]) && (m[0][0] > m[2][2]))
	{
		float s = sqrtf(1.0f + m[0][0] - m[1][1] - m[2][2]) * 2.0f;
		quaternion.w = (m[2][1] - m[1][2]) / s;
		quaternion.x = 0.25f * s;
		quaternion.y = (m[0][1] + m[1][0]) / s;
		quaternion.z = (m[0][2] + m[2][0]) / s;
	}
	else if (m[1][1] > m[2][2])
	{
		float s = sqrtf(1.0f + m[1][1] - m[0][0] - m[2][2]) * 2.0f;
		quaternion.w = (m[0][2] - m[2][0]) / s;
		quaternion.x = (m[0][1] + m[1][0]) / s;
		quaternion.y = 0.25f * s;
		quaternion.z = (m[1][2] + m[2][1]) / s;
	}
	else
	{
		float s = sqrtf(1.0f + m[2][2] - m[0][0] - m[1][1]) * 2.0f;
		quaternion.w = (m[1][0] - m[0][1]) / s;
		quaternion.x = (m[0][2] + m[2][0]) / s;
		quaternion.y = (m[1][2] + m[2][1]) / s;
		quaternion.z = 0.25f * s;
	}

	return quaternion;
}

void quaternionToRotationMatrix4f(const Quaternion& quaternion, mat4f dest)
{
	float x = quaternion.x;
	float y = quaternion.y;
	float z = quaternion.z;
	float w = quaternion.w;

	dest[0][0] = 1 - 2 * y * y - 2 * z * z;
	dest[0][1] = 2 * x * y - 2 * z * w;
	dest[0][2] = 2 * x * z + 2 * y * w;
	dest[0][3] = 0.0;

	dest[1][0] = 2 * x * y + 2 * z * w;
	dest[1][1] = 1 - 2 * x * x - 2 * z * z;
	dest[1][2] = 2 * y * z - 2 * x * w;
	dest[1][3] = 0.0;

	dest[2][0] = 2 * x * z - 2 * y * w;
	dest[2][1] = 2 * y * z + 2 * x * w;
	dest[2][2] = 1 - 2 * x * x - 2 * y * y;
	dest[2][3] = 0.0;

	dest[3][0] = 0.0;
	dest[3][1] = 0.0;
	dest[3][2] = 0.0;
	dest[3][3] = 1.0;
}

QuantizedQuaternion quantizeQuaternion(const Quaternion& quaternion, float scale)
{
	QuantizedQuaternion quantizedQuaternion;

	quantizedQuaternion.qy = static_cast<int16>(quaternion.y * scale);
	quantizedQuaternion.qz = static_cast<int16>(quaternion.z * scale);
	quantizedQuaternion.qw = static_cast<int16>(quaternion.w * scale);

	return quantizedQuaternion;
}

Quaternion dequantizeQuaternion(const QuantizedQuaternion& quantizedQuaternion, float scale)
{
	Quaternion quaternion;

	quaternion.x = 0.0f;
	quaternion.y = static_cast<float>(quantizedQuaternion.qy) / scale;
	quaternion.z = static_cast<float>(quantizedQuaternion.qz) / scale;
	quaternion.w = static_cast<float>(quantizedQuaternion.qw) / scale;

	float length = sqrtf(quaternion.y * quaternion.y +
		quaternion.z * quaternion.z +
		quaternion.w * quaternion.w);

	quaternion.y /= length;
	quaternion.z /= length;
	quaternion.w /= length;

	return quaternion;
}