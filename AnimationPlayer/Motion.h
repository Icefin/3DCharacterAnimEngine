#pragma once
#include <vector>
#include "CommonTypes.h"

struct Posture
{
	QuantizedQuaternion	rotation;
	vec3f				translation;
	float				length;
};

class Motion
{
public:
	Motion(int maxFrame);
	~Motion();

	Posture* getFrameBonePosture(int32 frame, int32 index);

private:
	std::vector<Posture> _keyFrameMotions;
};

