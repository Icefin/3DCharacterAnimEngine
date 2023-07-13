// author: Geonho Shin (icefin@pearlabyss.com)
#pragma once
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "CommonTypes.h"

struct CompressedAnimationData
{
	uint32				keyTime;
	QuantizedQuaternion	rotation;
};

struct AnimationData
{
	glm::quat			rotation;
	glm::vec3			translation;
};

class Motion
{
public:
				Motion(int32 totalBoneNumber, int32 totalFrameNumber);
				~Motion();

	void		updateKeyFrameTime(float deltaTime);
	void		resetKeyFrameTime(void) { _keyFrameTime = 0.0f; };

	glm::quat	getBoneAnimation(int32 boneIndex);
	void		setBoneAnimation(int32 boneIndex, std::vector<CompressedAnimationData>& boneAnim);

private :
	std::vector<std::vector<CompressedAnimationData>>	_keyFrameAnimations;
	int32												_maxFrameTime;
	float												_keyFrameTime;
};

