// author: Geonho Shin (icefin@pearlabyss.com)
#include "CustomMath.h"

QuantizedQuaternion quantizeQuaternion(const glm::quat quaternion)
{
	QuantizedQuaternion quantizedQuaternion;
    float quat[4] = { quaternion.w, quaternion.x, quaternion.y, quaternion.z };

    uint16 maxIndex = 0;
    float maxValue = std::abs(quat[0]);

    for (uint8 idx = 1; idx < 4; ++idx)
    {
        if (std::abs(quat[idx]) > maxValue)
        {
            maxValue = std::abs(quat[idx]);
            maxIndex = idx;
        }
    }

    quantizedQuaternion.largest = maxIndex;
    quantizedQuaternion.sign = static_cast<uint16>(quat[maxIndex] < 0.f ? 1 : 0);

    const static float Float2Int = 16383.f * sqrtf(2);
    const int permutation[4][3] = { {1, 2, 3}, {0, 2, 3}, {0, 1, 3}, {0, 1, 2} };
    const int* map = permutation[maxIndex];

    const int16 a = static_cast<int16>(quat[map[0]] * Float2Int);
    const int16 b = static_cast<int16>(quat[map[1]] * Float2Int);
    const int16 c = static_cast<int16>(quat[map[2]] * Float2Int);

    quantizedQuaternion.sign_a = static_cast<uint16>(a & 0x8000 ? 1 : 0);
    quantizedQuaternion.sign_b = static_cast<uint16>(b & 0x8000 ? 1 : 0);
    quantizedQuaternion.sign_c = static_cast<uint16>(c & 0x8000 ? 1 : 0);

    quantizedQuaternion.a = (quantizedQuaternion.sign_a == 0) ? static_cast<uint16>(a & 0x3FFF) : static_cast<uint16>(-a & 0x3FFF);
    quantizedQuaternion.b = (quantizedQuaternion.sign_b == 0) ? static_cast<uint16>(b & 0x3FFF) : static_cast<uint16>(-b & 0x3FFF);
    quantizedQuaternion.c = (quantizedQuaternion.sign_c == 0) ? static_cast<uint16>(c & 0x3FFF) : static_cast<uint16>(-c & 0x3FFF);

	return quantizedQuaternion;
}

glm::quat dequantizeQuaternion(const QuantizedQuaternion& quantizedQuaternion)
{
	glm::quat quaternion;

    const static float Int2Float = 1.f / (16383.f * sqrtf(2));
    const int Mapping[4][3] = { {1, 2, 3}, {0, 2, 3}, {0, 1, 3}, {0, 1, 2} };

    uint16 largest = quantizedQuaternion.largest;
    float quat[4] = { 0.f, 0.f, 0.f, 0.f };
    const int* map = Mapping[largest];

    quat[largest] = (quantizedQuaternion.sign == 0) ? 1.0f : -1.0f;

    quat[map[0]] = (quantizedQuaternion.sign_a == 0) ? static_cast<float>(quantizedQuaternion.a * Int2Float) : -static_cast<float>(quantizedQuaternion.a * Int2Float);
    quat[map[1]] = (quantizedQuaternion.sign_b == 0) ? static_cast<float>(quantizedQuaternion.b * Int2Float) : -static_cast<float>(quantizedQuaternion.b * Int2Float);
    quat[map[2]] = (quantizedQuaternion.sign_c == 0) ? static_cast<float>(quantizedQuaternion.c * Int2Float) : -static_cast<float>(quantizedQuaternion.c * Int2Float);
    quat[largest] *= sqrtf(1 - ((quat[map[0]] * quat[map[0]]) + (quat[map[1]] * quat[map[1]]) + (quat[map[2]] * quat[map[2]])));

    quaternion.w = quat[0];
    quaternion.x = quat[1];
    quaternion.y = quat[2];
    quaternion.z = quat[3];

	return glm::normalize(quaternion);
}

float interpolateCatmullRomSpline(float p0, float p1, float p2, float p3, float t)
{
	float c0 = 0.5f * 2 * p1;
	float c1 = 0.5f * t * (-p0 + p2);
	float c2 = 0.5f * t * t * (2 * p0 - 5 * p1 + 4 * p2 - p3);
	float c3 = 0.5f * t * t * t * (-p0 + 3 * p1 - 3 * p2 + p3);
	return (c0 + c1 + c2 + c3);
}