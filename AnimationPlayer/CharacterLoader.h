// author: Geonho Shin (icefin@pearlabyss.com)
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "CommonTypes.h"
#include "Character.h"
#include "ASFParser.h"
#include "AMCParser.h"

#pragma once
class CharacterLoader
{
public:
	void		loadCharacter(Character& character, std::string& asf, std::string& amc);

private:
	Skeleton*	generateSkeleton(ASFData* asfData);
	void		setupToParentMatrix(ASFData* asfData);
	void		computeToParentMatrix(ASFBone* parent, ASFBone* child);
	Bone*		generateBone(ASFBone* boneData);
	void		setupSkeletonHierarchy(std::vector<Bone*>& boneList, ASFData* asfData);

	Motion*		generateMotion(AMCData* amcData, int32 totalBoneNumber);
	std::vector<CompressedAnimation>	compressMotion(std::vector<AnimationData> data);

	ASFParser	_asfParser;
	AMCParser	_amcParser;
};