#pragma once
#include <vector>
#include "CommonTypes.h"

struct Posture
{
	QuantizedQuaternion	rotation;
	vec3f				translation;
	float				location;
};

class Motion
{
public:
	Motion(int maxFrame);
	~Motion();

	Posture* getFrameBonePosture(int32 frame, int32 index) { return &(_keyFrameMotions[frame][index]); };

private:
	std::vector<std::vector<Posture>> _keyFrameMotions;
};

