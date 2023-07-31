#pragma once
#include <string>
#include <vector>

#include "Motion.h"

constexpr float kBlendTime = 30.0f;

enum class AnimationState : uint8
{
	IDLE = 0,
	FORWARD,
	BACKWARD,
	RUN,
	JUMP,
	ATTACK
};

struct LayerInfo
{
	int32			parentLayerIndex;

	int32			animationRootBoneIndex;

	AnimationState	prevState = AnimationState::IDLE;
	float			prevMotionTime = 0.0f;

	AnimationState	currentState = AnimationState::IDLE;
	float			currentMotionTime = 0.0f;

	float			blendWeight = kBlendTime;
	int32			maxFrameTime = 1'000;
	bool			isLooping = true;
};

class Animator
{
public :
				Animator(std::vector<Motion*>& motionList);
				~Animator();

	void		update(AnimationState state, float deltaTime);

	glm::quat	getJointAnimation(int32 jointIndex);

private :
	void		updateAnimationLayerListState(AnimationState state, float deltaTime);
	std::vector<Motion*>	_motionList;
	std::vector<LayerInfo>	_animationLayerList;
};
