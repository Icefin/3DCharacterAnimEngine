#pragma once
#include <string>
#include <vector>

#include "Motion.h"

enum class AnimationState : uint8
{
	IDLE = 0,
	FORWARD,
	BACKWARD,
	RUN,
	JUMP,
	ATTACK
};

constexpr float kBlendTime = 30.0f;

struct LayerInfo
{
	int32			parentLayerIndex;

	int32			animationRootBoneIndex;

	AnimationState	prevState = AnimationState::IDLE;
	float			prevMotionTime = 0.0f;

	AnimationState	currentState = AnimationState::IDLE;
	float			currentMotionTime = 0.0f;

	float			blendWeight = kBlendTime;
	int32			maxFrameTime = 0;
	bool			isLooping = true;
};

class Animator
{
public :
				Animator();
				~Animator();

	void		update(AnimationState state, float deltaTime);

	glm::quat	getBoneAnimation(int32 boneIndex);

private :
	void	updateAnimationLayerListState(AnimationState state, float deltaTime);
	std::vector<Motion*>	_motionList;
	std::vector<LayerInfo>	_animationLayerList;
};
