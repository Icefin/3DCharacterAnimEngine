#pragma once
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "CommonTypes.h"

struct Posture
{
	QuantizedQuaternion	rotation;
	glm::vec3			translation;
	float				location;
};

class Motion
{
public:
	Motion(int32 maxFrame);
	~Motion();

	Posture* getFrameBonePosture(int32 frame, int32 index) { return &(_keyFrameMotions[frame][index]); };

private:
	std::vector<std::vector<Posture>>	_keyFrameMotions;
};

