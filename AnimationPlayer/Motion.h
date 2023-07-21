// author: Geonho Shin (icefin@pearlabyss.com)
#pragma once
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "CommonTypes.h"

struct CompressedPose
{
	uint32				keyTime;
	QuantizedQuaternion	rotation;
};

struct Pose
{
	glm::quat			rotation;
	glm::vec3			translation;
};

class Motion
{
public:
				Motion(int32 totalBoneNumber, int32 totalFrameNumber);
				~Motion();

	void		updateFrameTime(float deltaTime);
	void		resetFrameTime(void) { _frameTime = 0.0f; };

	glm::quat	getJointPose(int32 boneIndex);
	void		setJointPose(int32 boneIndex, std::vector<CompressedPose>& boneAnim);

private :
	std::vector<std::vector<CompressedPose>>	_keyFramePoseList;
	int32										_maxFrameTime;
	float										_frameTime;
};

