// author: Geonho Shin (icefin@pearlabyss.com)
#include "Motion.h"
#include "Transform.h"

Motion::Motion(int32 totalBoneNumber, int32 totalFrameNumber)
{
	_keyFrameMotions = std::vector<std::vector<Posture>>(totalBoneNumber, std::vector<Posture>(totalFrameNumber));
	_keyFrameAnimations = std::vector<std::vector<CompressedAnimation>>(totalBoneNumber);
}

Motion::~Motion()
{
	__noop;
}

void		Motion::updateKeyFrameTime(float deltaTime)
{
	_keyFrameTime += deltaTime;
	if (_keyFrameTime >= _maxFrameTime)
		_keyFrameTime = 0;
	_prevKeyFrameIndex = 1;
}

Posture* Motion::getBonePostureAtFrame(int32 boneIndex, int32 frame)
{
	return &(_keyFrameMotions[boneIndex][frame]);
}

glm::quat	Motion::getBoneAnimation(int32 boneIndex)
{
	std::vector<CompressedAnimation>& boneAnimation = _keyFrameAnimations[boneIndex];
	
	if (boneAnimation[_prevKeyFrameIndex + 1].keyTime <= _keyFrameTime)
		_prevKeyFrameIndex++;

	//dequnatizeQuaternion
	glm::quat p0 = dequantizeQuaternion(boneAnimation[_prevKeyFrameIndex - 1].rotation);
	glm::quat p1 = dequantizeQuaternion(boneAnimation[_prevKeyFrameIndex].rotation);
	glm::quat p2 = dequantizeQuaternion(boneAnimation[_prevKeyFrameIndex + 1].rotation);
	glm::quat p3 = dequantizeQuaternion(boneAnimation[_prevKeyFrameIndex + 2].rotation);
	
	//interpolate Catmull-Rom
	int32 startKeyTime = boneAnimation[_prevKeyFrameIndex].keyTime;
	int32 endKeyTime = boneAnimation[_prevKeyFrameIndex + 1].keyTime;
	
	float t = (_keyFrameTime - startKeyTime) / (endKeyTime - startKeyTime);
	float x = interpolateCatmullRomSpline(p0.x, p1.x, p2.x, p3.x, t);
	float y = interpolateCatmullRomSpline(p0.y, p1.y, p2.y, p3.y, t);
	float z = interpolateCatmullRomSpline(p0.z, p1.z, p2.z, p3.z, t);
	float w = interpolateCatmullRomSpline(p0.w, p1.w, p2.w, p3.w, t);

	//return rotation
	return glm::quat(x, y, z, w);
}

void		Motion::setBoneAnimation(int32 boneIndex, std::vector<CompressedAnimation>& boneAnim)
{
	_keyFrameAnimations[boneIndex] = boneAnim;
}