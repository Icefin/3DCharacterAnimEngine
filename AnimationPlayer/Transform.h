// author: Geonho Shin (icefin@pearlabyss.com)
#pragma once
#include <math.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

#include "CommonTypes.h"

#define QUANT_SCALE 32767.0f

QuantizedQuaternion quantizeQuaternion(glm::quat quaternion, float scale);
glm::quat dequantizeQuaternion(const QuantizedQuaternion& quantizedQuaternion, float scale);