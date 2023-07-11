// author: Geonho Shin (icefin@pearlabyss.com)
#include "Motion.h"

Motion::Motion(int32 totalBoneNumber, int32 totalFrameNumber)
{
	_keyFrameMotions = std::vector<std::vector<Posture>>(totalBoneNumber, std::vector<Posture>(totalFrameNumber));
}

Motion::~Motion()
{
	__noop;
}

Posture* Motion::getBonePostureAtFrame(int32 boneIndex, int32 frame)
{
	return &(_keyFrameMotions[boneIndex][frame]);
}