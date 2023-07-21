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
	Character*	loadCharacter(std::string& skeleton, std::vector<std::string>& motions);

private:
	Skeleton*	generateSkeleton(ASFData* asfData);
	void		setupToParentMatrix(ASFData* asfData);
	void		computeToParentMatrix(ASFBone* parent, ASFBone* child);
	Joint*		generateJoint(ASFBone* boneData);

	Motion*		generateMotion(AMCData* amcData, int32 totalBoneNumber);
	std::vector<CompressedPose>	compressAnimation(std::vector<Pose>& data);

	ASFParser	_asfParser;
	AMCParser	_amcParser;
};