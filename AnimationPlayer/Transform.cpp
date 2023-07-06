// author: Geonho Shin (icefin@pearlabyss.com)
#include "Transform.h"

QuantizedQuaternion quantizeQuaternion(const glm::quat quaternion, const float scale)
{
	QuantizedQuaternion quantizedQuaternion;

	quantizedQuaternion.qy = static_cast<int16>(quaternion.y * scale);
	quantizedQuaternion.qz = static_cast<int16>(quaternion.z * scale);
	quantizedQuaternion.qw = static_cast<int16>(quaternion.w * scale);
	if (quaternion.x < 0)
	{
		quantizedQuaternion.qy *= -1.0;
		quantizedQuaternion.qz *= -1.0;
		quantizedQuaternion.qw *= -1.0;
	}

	return quantizedQuaternion;
}

glm::quat dequantizeQuaternion(const QuantizedQuaternion& quantizedQuaternion, const float scale)
{
	glm::quat quaternion;

	quaternion.y = static_cast<float>(quantizedQuaternion.qy) / scale;
	quaternion.z = static_cast<float>(quantizedQuaternion.qz) / scale;
	quaternion.w = static_cast<float>(quantizedQuaternion.qw) / scale;

	quaternion.x = sqrtf(1.0f - ((quaternion.y * quaternion.y) + (quaternion.z * quaternion.z) + (quaternion.w * quaternion.w)));

	return quaternion;
}