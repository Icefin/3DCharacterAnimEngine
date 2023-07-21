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

	glm::quat	getJointPose(int32 boneIndex, float frameTime);
	int32		getMaxFrameTime(void) { return _maxFrameTime; };
	void		setJointPose(int32 boneIndex, std::vector<CompressedPose>& boneAnim);

private :
	std::vector<std::vector<CompressedPose>>	_keyFramePoseList;
	int32										_maxFrameTime;
};

