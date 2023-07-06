#include <string>
#include <queue>

#include "CharacterLoader.h"
#include "Transform.h"

#define SCALE 0.25f

void CharacterLoader::loadCharacter(Character& character, std::string& asf, std::string& amc)
{
	ASFData* asfData = _asfParser.readASF(asf);
	AMCData* amcData = _amcParser.readAMC(amc, asfData);

	Skeleton* skeleton = generateSkeleton(asfData);
	Motion* motion = generateMotion(amcData);

	character.initialize(skeleton, motion);
}

Skeleton* CharacterLoader::generateSkeleton(ASFData* asfData)
{
	rotateBoneDirectionToBoneSpace(asfData);
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

void	CharacterLoader::rotateBoneDirectionToBoneSpace(ASFData* asfData)
{
	std::unordered_map<std::string, ASFBone>& boneMap = asfData->boneMap;

	
	for (auto& elem : boneMap)
	{
		glm::mat4 matrix = glm::mat4(1.0f);
		ASFBone& bone = elem.second;

		matrix = glm::rotate(matrix, bone.orientation.x, { 1.0f, 0.0f, 0.0f });
		matrix = glm::rotate(matrix, bone.orientation.y, { 0.0f, 1.0f, 0.0f });
		matrix = glm::rotate(matrix, bone.orientation.z, { 0.0f, 0.0f, 1.0f });
		
		bone.direction = glm::vec3(matrix * glm::vec4(bone.direction, 0.0f));
		bone.direction = asfData->length * (bone.direction * bone.length) * SCALE;
	}
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
	}
}

void	CharacterLoader::computeToParentMatrix(ASFBone* parent, ASFBone* child)
{
	glm::mat4 matrix = glm::mat4(1.0f);

	matrix = glm::rotate(matrix, parent->orientation.x, { 1.0f, 0.0f, 0.0f });
	matrix = glm::rotate(matrix, parent->orientation.y, { 0.0f, 1.0f, 0.0f });
	matrix = glm::rotate(matrix, parent->orientation.z, { 0.0f, 0.0f, 1.0f });

	matrix = glm::rotate(matrix, -child->orientation.z, { 0.0f, 0.0f, 1.0f });
	matrix = glm::rotate(matrix, -child->orientation.y, { 0.0f, 1.0f, 0.0f });
	matrix = glm::rotate(matrix, -child->orientation.x, { 1.0f, 0.0f, 0.0f });
	
	child->toParent = matrix;
}

Bone* CharacterLoader::generateBone(ASFBone* boneData)
{
	Bone* newBone = new Bone();
	
	newBone->index = boneData->boneIndex;
	newBone->translation = boneData->direction;
	
	glm::quat quat = glm::quat(boneData->toParent);
	QuantizedQuaternion qquat = quantizeQuaternion(quat, QUANT_SCALE);

	newBone->toParent = qquat;

	return newBone;
}

void	CharacterLoader::setupSkeletonHierarchy(std::vector<Bone*>& boneList, ASFData* asfData)
{
	for (auto& elem : asfData->boneMap)
	{
		ASFBone& asfBone = elem.second;
		int32 index = asfBone.boneIndex;
		for (int32 i = 0; i < asfBone.childList.size(); i++)
			boneList[index]->childList.push_back(boneList[asfBone.childList[i]->boneIndex]);
	}
}

Motion* CharacterLoader::generateMotion(AMCData* amcData)
{
	__noop;
	return NULL;
}