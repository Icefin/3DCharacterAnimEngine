// author: Geonho Shin (icefin@pearlabyss.com)
#pragma once
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "CommonTypes.h"

struct Posture
{
	QuantizedQuaternion	rotation;
	glm::mat4			TEST_ROTATION;
	glm::vec3			translation;
	float				location;
};

class Motion
{
public:
	Motion(int32 totalBoneNumber, int32 totalFrameNumber);
	~Motion();

	Posture* getBonePostureAtFrame(int32 boneIndex, int32 frame) { return &(_keyFrameMotions[boneIndex][frame]); };


	std::vector<std::vector<Posture>>	_keyFrameMotions;
};

