// author: Geonho Shin (icefin@pearlabyss.com)
#pragma once
#include <math.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

#include "CommonTypes.h"

#define QUANT_SCALE 32767.0f

QuantizedQuaternion quantizeQuaternion(const glm::quat quaternion, const float scale = QUANT_SCALE);
glm::quat dequantizeQuaternion(const QuantizedQuaternion& quantizedQuaternion, const float scale = QUANT_SCALE);

float interpolateCatmullRomSpline(float p0, float p1, float p2, float p3, float t);