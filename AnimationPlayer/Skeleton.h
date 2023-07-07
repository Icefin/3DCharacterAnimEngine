// author: Geonho Shin (icefin@pearlabyss.com)
#pragma once
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "CommonTypes.h"

struct Bone
{
	std::vector<Bone*>		childList;

	int32					index;
	glm::vec3				translation;
	QuantizedQuaternion		toParent;

	glm::mat4				TEST_PARENT;
};

class Skeleton
{
public:
	Skeleton(Bone* root) : _root(root) {};
	~Skeleton();

	Bone* getRoot(void) { return _root; };

private :
	Bone* _root;
};

