// author: Geonho Shin (icefin@pearlabyss.com)
#include <string>
#include <queue>

#include "CharacterLoader.h"
#include "Transform.h"

constexpr float CHARACTER_SCALE = 0.1f;

static void		TEST_PRINT_BONE_INFO(ASFData* asfData)
{
	std::unordered_map<std::string, ASFBone>& boneMap = asfData->boneMap;

	for (auto& elem : boneMap)
	{
		printf("Bone Name : %s\n", elem.first.c_str());
		printf("Bone direction : %f %f %f\n",elem.second.direction.x, elem.second.direction.y, elem.second.direction.z);
		printf("Bone length : %f\n", elem.second.length);
		printf("Bone Axis : %f %f %f\n", elem.second.orientation.x, elem.second.orientation.y, elem.second.orientation.z);
		printf("\n");
	}
}

static void		TEST_PRINT_POSTURE_INFO(AMCData* amcData)
{
	std::vector<std::vector<AMCPosture>>& boneMotions = amcData->boneMotions;

	for (int32 i = 18; i < 19; i++)
	{
		for (int32 j = 0; j < 500; j++)
		{
			printf("Rotation of (ID : %d, frame : %d) : %f %f %f\n", i, j, glm::degrees(boneMotions[i][j].frameRotation.x), glm::degrees(boneMotions[i][j].frameRotation.y), glm::degrees(boneMotions[i][j].frameRotation.z));
		}
	}
}

void CharacterLoader::loadCharacter(Character& character, std::string& asf, std::string& amc)
{
	ASFData* asfData = _asfParser.readASF(asf);
	AMCData* amcData = _amcParser.readAMC(amc, asfData);

	//curve fitting logic comes here

	Skeleton* skeleton = generateSkeleton(asfData);
	TEST_PRINT_BONE_INFO(asfData);
	Motion* motion = generateMotion(amcData, asfData->totalBoneNumber);
	TEST_PRINT_POSTURE_INFO(amcData);

 	character.initialize(skeleton, motion);
}

Skeleton* CharacterLoader::generateSkeleton(ASFData* asfData)
{
	setupToParentMatrix(asfData);

	std::vector<Bone*> boneList(asfData->totalBoneNumber);
	for (auto& elem : asfData->boneMap)
	{
		int32 index = elem.second.boneIndex;
		boneList[index] = generateBone(&elem.second);
	}

	setupSkeletonHierarchy(boneList, asfData);
	
	Skeleton* skeleton = new Skeleton(boneList[0]);
	return skeleton;
}

void	CharacterLoader::setupToParentMatrix(ASFData* asfData)
{
	ASFBone* root = &(asfData->boneMap["root"]);
	root->toParent = glm::mat4(1.0f);

	std::queue<ASFBone*> qBone;
	qBone.push(root);
	while (qBone.empty() == false)
	{
		ASFBone* currentBone = qBone.front(); qBone.pop();

		for (ASFBone* childBone : currentBone->childList)
		{
			computeToParentMatrix(currentBone, childBone);
			qBone.push(childBone);
		}
		if (currentBone->childList.empty() == true)
		{
			//leaf node process (dummy generate)
		}
	}
}

void	CharacterLoader::computeToParentMatrix(ASFBone* parent, ASFBone* child)
{
	glm::mat4 rotation = glm::mat4(1.0f);

	rotation = glm::rotate(rotation, -parent->orientation.x, { 1, 0, 0 });
	rotation = glm::rotate(rotation, -parent->orientation.y, { 0, 1, 0 });
	rotation = glm::rotate(rotation, -parent->orientation.z, { 0, 0, 1 });

	rotation = glm::rotate(rotation, child->orientation.z, { 0, 0, 1 });
	rotation = glm::rotate(rotation, child->orientation.y, { 0, 1, 0 });
	rotation = glm::rotate(rotation, child->orientation.x, { 1, 0, 0 });

	glm::mat4 translation = glm::mat4(1.0);
	glm::mat4 toChildRotation = glm::mat4(1.0);

	toChildRotation = glm::rotate(toChildRotation, -child->orientation.x, { 1, 0, 0 });
	toChildRotation = glm::rotate(toChildRotation, -child->orientation.y, { 0, 1, 0 });
	toChildRotation = glm::rotate(toChildRotation, -child->orientation.z, { 0, 0, 1 });

	glm::vec3 offset = glm::vec3(toChildRotation * glm::vec4(parent->direction, 0.0f)) * parent->length * CHARACTER_SCALE;
	translation = glm::translate(translation, offset);

	child->toParent = rotation * translation;
}

Bone* CharacterLoader::generateBone(ASFBone* boneData)
{
	Bone* newBone = new Bone();
	
	newBone->index = boneData->boneIndex;
	newBone->toParent = boneData->toParent;

	return newBone;
}

void	CharacterLoader::setupSkeletonHierarchy(std::vector<Bone*>& boneList, ASFData* asfData)
{
	for (auto& elem : asfData->boneMap)
	{
		ASFBone& asfBone = elem.second;
		int32 index = asfBone.boneIndex;
		for (int32 i = 0; i < asfBone.childList.size(); ++i)
			boneList[index]->childList.push_back(boneList[asfBone.childList[i]->boneIndex]);
	}
}

Motion* CharacterLoader::generateMotion(AMCData* amcData, int32 totalBoneNumber)
{
	int32	totalFrameNumber = amcData->totalFrameNumber;
	std::vector<std::vector<AMCPosture>>& motionDatas = amcData->boneMotions;

	Motion* motion = new Motion(totalBoneNumber, totalFrameNumber);
	for (int32 boneIndex = 0; boneIndex < totalBoneNumber; ++boneIndex)
	{
		std::vector<AMCPosture>& posture = motionDatas[boneIndex];
		if (posture.size() == 0)
			continue;
		for (int32 frame = 0; frame < totalFrameNumber; ++frame)
		{
			glm::mat4 rotation = glm::mat4(1.0);
			rotation = glm::rotate(rotation, posture[frame].frameRotation.z, { 0, 0, 1 });
			rotation = glm::rotate(rotation, posture[frame].frameRotation.y, { 0, 1, 0 });
			rotation = glm::rotate(rotation, posture[frame].frameRotation.x, { 1, 0, 0 });

			glm::mat4 translation = glm::mat4(1.0);
			translation = glm::translate(translation, posture[frame].frameTranslation);

			motion->_keyFrameMotions[boneIndex][frame].rotation = rotation;
			motion->_keyFrameMotions[boneIndex][frame].translation = translation * CHARACTER_SCALE;
		}
	}
	return motion;
}