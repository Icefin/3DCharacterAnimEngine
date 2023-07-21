// author: Geonho Shin (icefin@pearlabyss.com)
#pragma once
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "CommonTypes.h"

struct Joint
{
	int32		parentIndex;

	glm::mat4	jointToParentMatrix = glm::mat4(1.0f);
	glm::mat4	modelToJointMatrix = glm::mat4(1.0f);
};

class Skeleton
{
public:
			Skeleton(std::vector<Joint*>& joints);
			~Skeleton();

	Joint*	getRoot(void) { return _jointList[0]; };
	Joint*	getJoint(int32 index) { return _jointList[index]; };
	int32	getJointNumber(void) { return _jointList.size(); };

private :
	std::vector<Joint*> _jointList;
};

