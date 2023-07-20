// author: Geonho Shin (icefin@pearlabyss.com)
#pragma once
#include <math.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

#include "CommonTypes.h"


QuantizedQuaternion quantizeQuaternion(const glm::quat quaternion);
glm::quat			dequantizeQuaternion(const QuantizedQuaternion& quantizedQuaternion);

float				interpolateCatmullRomSpline(float p0, float p1, float p2, float p3, float t);