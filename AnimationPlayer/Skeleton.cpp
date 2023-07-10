// author: Geonho Shin (icefin@pearlabyss.com)
#include <queue>

#include "Skeleton.h"

Skeleton::~Skeleton()
{
	std::queue<Bone*> qBone;
	qBone.push(_root);

	while (qBone.empty() == false)
	{
		Bone* curr = qBone.front(); qBone.pop();

		for (Bone* child : curr->childList)
			qBone.push(child);
		delete curr;
	}
}