// author: Geonho Shin (icefin@pearlabyss.com)
#pragma once
#include <math.h>

#include "CommonTypes.h"

#define PI			3.141592f
#define QUANT_SCALE 32767.0f

void	copyMatrix4f(const mat4f src, mat4f dest);
void	multiplyMatrix4fToMatrix4f(const mat4f a, const mat4f b, mat4f dest);
void	multiplyMatrix4fToVector3f(const mat4f a, const vec3f& b, vec3f& dest);

void	setIdentityMatrix4f(mat4f dest);
void	setRotationMatrix4fX(float radian, mat4f dest);
void	setRotationMatrix4fY(float radian, mat4f dest);
void	setRotationMatrix4fZ(float radian, mat4f dest);

float	magnitudeVector3f(const vec3f& u);
float	dotProductVector3f(const vec3f& u, const vec3f& v);
void	crossProductVector3f(const vec3f& u, const vec3f& v, vec3f& dest);
float	angleBetweenVector3f(const vec3f& u, const vec3f& v);

float	degreeToRadian(float degree);
float	radianToDegree(float radian);

Quaternion rotationMatrix4fToQuaternion(mat4f m);
void quaternionToRotationMatrix4f(const Quaternion& quaternion, mat4f dest);

QuantizedQuaternion quantizeQuaternion(const Quaternion& quaternion, float scale);
Quaternion dequantizeQuaternion(const QuantizedQuaternion& quantizedQuaternion, float scale);