#include <string>
#include <queue>

#include "CharacterLoader.h"
#include "Transform.h"

Character* CharacterLoader::loadCharacter(std::string& asf, std::string& amc)
{
	ASFData* asfData = _asfParser.readASF(asf);
	AMCData* amcData = _amcParser.readAMC(amc, asfData);

	Skeleton* skeleton = generateSkeleton(asfData);
	Motion* motion = generateMotion(amcData);

	Character* character = new Character(skeleton, motion);
	return character;
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

	mat4f Rx, Ry, Rz;
	vec3f temp1, temp2;
	for (auto& elem : boneMap)
	{
		ASFBone& bone = elem.second;

		setRotationMatrix4fX(-bone.orientation.x, Rx);
		setRotationMatrix4fY(-bone.orientation.y, Ry);
		setRotationMatrix4fZ(-bone.orientation.z, Rz);

		multiplyMatrix4fToVector3f(Rz, bone.direction, temp1);
		multiplyMatrix4fToVector3f(Ry, temp1, temp2);
		multiplyMatrix4fToVector3f(Rx, temp2, bone.direction);
	}
}

void	CharacterLoader::setupToParentMatrix(ASFData* asfData)
{
	ASFBone* root = &(asfData->boneMap["root"]);
	setIdentityMatrix4f(root->toParent);

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
	mat4f Rx, Ry, Rz;
	mat4f temp1, temp2, res;

	setRotationMatrix4fX(-child->orientation.x, Rx);
	setRotationMatrix4fY(-child->orientation.y, Ry);
	setRotationMatrix4fZ(-child->orientation.z, Rz);

	multiplyMatrix4fToMatrix4f(Rx, Ry, res);
	multiplyMatrix4fToMatrix4f(res, Rz, temp1);

	setRotationMatrix4fX(parent->orientation.x, Rx);
	setRotationMatrix4fY(parent->orientation.y, Ry);
	setRotationMatrix4fZ(parent->orientation.z, Rz);

	multiplyMatrix4fToMatrix4f(Rz, Ry, res);
	multiplyMatrix4fToMatrix4f(res, Rx, temp2);

	multiplyMatrix4fToMatrix4f(temp1, temp2, res);
	//revCx revCy revCz Pz Py Px
	copyMatrix4f(res, child->toParent);
}

Bone* CharacterLoader::generateBone(ASFBone* boneData)
{
	Bone* newBone = new Bone();
	
	newBone->index = boneData->boneIndex;
	newBone->translation = boneData->direction;
	
	Quaternion quat = rotationMatrix4fToQuaternion(boneData->toParent);
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