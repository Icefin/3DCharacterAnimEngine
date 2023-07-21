// author: Geonho Shin (icefin@pearlabyss.com)
#include <queue>

#include "Skeleton.h"

Skeleton::Skeleton(std::vector<Joint*>& joints)
{
	_jointList = joints;
}

Skeleton::~Skeleton()
{
	int jointNumber = _jointList.size();
	for (int i = 0; i < jointNumber; ++i)
		delete _jointList[i];
}