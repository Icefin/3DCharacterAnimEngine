// author: Geonho Shin (icefin@pearlabyss.com)
#include "Motion.h"
#include "Transform.h"

Motion::Motion(int32 totalBoneNumber, int32 totalFrameNumber)
{
	_keyFrameAnimations = std::vector<std::vector<CompressedAnimationData>>(totalBoneNumber);
	_maxFrameTime = totalFrameNumber;
}

Motion::~Motion()
{
	__noop;
}

void Motion::updateKeyFrameTime(float deltaTime)
{
	_frameTime += deltaTime;
	if (_frameTime >= _maxFrameTime)
		_frameTime = 0;
}

glm::quat Motion::getBoneAnimation(int32 boneIndex)
{
	std::vector<CompressedAnimationData>& boneAnimation = _keyFrameAnimations[boneIndex];
	if (boneAnimation.empty() == true)
		return glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

	int32 targetFrameIndex = 1;
	int32 lowerFrameIndex = 1, upperFrameIndex = boneAnimation.size() - 3;
	while (lowerFrameIndex <= upperFrameIndex)
	{
		int32 midFrameIndex = lowerFrameIndex + (upperFrameIndex - lowerFrameIndex) / 2;

		if (boneAnimation[midFrameIndex].keyTime <= _frameTime)
		{
			targetFrameIndex = midFrameIndex;
			lowerFrameIndex = midFrameIndex + 1;
		}
		else
			upperFrameIndex = midFrameIndex - 1;
	}

	//dequnatizeQuaternion
	glm::quat p0 = dequantizeQuaternion(boneAnimation[targetFrameIndex - 1].rotation);
	glm::quat p1 = dequantizeQuaternion(boneAnimation[targetFrameIndex].rotation);
	glm::quat p2 = dequantizeQuaternion(boneAnimation[targetFrameIndex + 1].rotation);
	glm::quat p3 = dequantizeQuaternion(boneAnimation[targetFrameIndex + 2].rotation);
	
	//interpolate Catmull-Rom
	int32 startKeyTime = boneAnimation[targetFrameIndex].keyTime;
	int32 endKeyTime = boneAnimation[targetFrameIndex + 1].keyTime;
	
	float t = (_frameTime - startKeyTime) / (endKeyTime - startKeyTime);
	float x = interpolateCatmullRomSpline(p0.x, p1.x, p2.x, p3.x, t);
	float y = interpolateCatmullRomSpline(p0.y, p1.y, p2.y, p3.y, t);
	float z = interpolateCatmullRomSpline(p0.z, p1.z, p2.z, p3.z, t);
	float w = interpolateCatmullRomSpline(p0.w, p1.w, p2.w, p3.w, t);

	//return rotation
	return glm::normalize(glm::quat(w, x, y, z));
}

void Motion::setBoneAnimation(int32 boneIndex, std::vector<CompressedAnimationData>& boneAnim)
{
	_keyFrameAnimations[boneIndex] = boneAnim;
}