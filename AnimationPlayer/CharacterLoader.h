// author: Geonho Shin (icefin@pearlabyss.com)
#include "CommonTypes.h"
#include "Character.h"
#include "ASFParser.h"
#include "AMCParser.h"

#pragma once
class CharacterLoader
{
public:
	Character* loadCharacter(std::string& asf, std::string& amc);

private:
	ASFParser _asfParser;
	AMCParser _amcParser;

	Skeleton*	generateSkeleton(ASFData* asfData);
	void		rotateBoneDirectionToBoneSpace(ASFData* asfData);
	void		setupToParentMatrix(ASFData* asfData);
	void		computeToParentMatrix(ASFBone* parent, ASFBone* child);
	Bone*		generateBone(ASFBone* boneData);
	void		setupSkeletonHierarchy(std::vector<Bone*>& boneList, ASFData* asfData);

	Motion* generateMotion(AMCData* amcData);
	//curve fitting...
};