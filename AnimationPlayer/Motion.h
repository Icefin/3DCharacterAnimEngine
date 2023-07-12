// author: Geonho Shin (icefin@pearlabyss.com)
#pragma once
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "CommonTypes.h"

struct CompressedAnimation
{
	uint32				keyTime;
	QuantizedQuaternion	rotation;
};

struct AnimationData
{
	glm::quat			rotation;
	glm::vec3			translation;
};

struct Posture
{
	//glm::mat4			rotation = glm::mat4(1.0f);
	QuantizedQuaternion	qrotation = {0, 0, 32767};
	glm::mat4			translation = glm::mat4(1.0f);
};

class Motion
{
public:
	Motion(int32 totalBoneNumber, int32 totalFrameNumber);
	~Motion();

	void		updateKeyFrameTime(float deltaTime);

	Posture*	getBonePostureAtFrame(int32 boneIndex, int32 frame);
	glm::quat	getBoneAnimation(int32 boneIndex);
	void		setBoneAnimation(int32 boneIndex, std::vector<CompressedAnimation>& boneAnim);

	std::vector<std::vector<Posture>>	_keyFrameMotions;

private :
	int32											_maxFrameTime;
	std::vector<std::vector<CompressedAnimation>>	_keyFrameAnimations;
	int32											_prevKeyFrameIndex;
	int32											_keyFrameTime;
};

