#pragma once
#include <vector>
#include "CommonTypes.h"

struct Bone
{
	std::vector<Bone*>  childList;

	int32				index;
	vec3f				translation;
	QuantizedQuaternion toParent;
};

class Skeleton
{
public:
	Skeleton(Bone* root) : _root(root) {};
	~Skeleton();

	Bone* getRoot() { return _root; };

private :
	Bone* _root;
};

