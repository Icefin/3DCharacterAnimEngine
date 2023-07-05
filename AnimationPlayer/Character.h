// author: Geonho Shin (icefin@pearlabyss.com)
#pragma once
#include "Skeleton.h"
#include "Motion.h"

class Character
{
public:
	Character();
	Character(Skeleton* skeleton, Motion* motion);
	~Character();

private:
	Skeleton* _skeleton;
	Motion* _motion;
};

