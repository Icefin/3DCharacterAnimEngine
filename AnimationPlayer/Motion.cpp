#include "Motion.h"

Motion::Motion(int maxFrame)
{
	_keyFrameMotions = std::vector<std::vector<Posture>>(maxFrame);
}

