#include "Animator.h"

Animator::Animator(std::vector<Motion*>& motionList)
{
	int32 motionNumber = motionList.size();
	_motionList.resize(motionNumber);
	for (int32 idx = 0; idx < motionNumber; ++idx)
		_motionList[idx] = motionList[idx];

	_animationLayerList.resize(2);

	_animationLayerList[0].parentLayerIndex = -1;
	_animationLayerList[0].animationRootBoneIndex = 0;

	_animationLayerList[1].parentLayerIndex = 0;
	_animationLayerList[1].animationRootBoneIndex = 11;

	//Replace anywhere...
	_motionList[4]->_isLooping = false;
	_motionList[5]->_isLooping = false;
}

Animator::~Animator()
{
	__noop;
}

void Animator::update(AnimationState state, float deltaTime)
{
	updateAnimationLayerListState(state, deltaTime);
}

static void convertLayerState(LayerInfo& layer, AnimationState state, Motion* motion)
{
	if (layer.isLooping == false && layer.currentMotionTime < layer.maxFrameTime)
		return;

	layer.prevState = layer.currentState;
	layer.currentState = state;

	layer.blendWeight = kBlendTime - layer.blendWeight;
	layer.prevMotionTime = layer.currentMotionTime;
	layer.currentMotionTime = 0.0f;

	layer.maxFrameTime = motion->getMaxFrameTime();
	layer.isLooping = motion->_isLooping;
}

static void updateLayerState(LayerInfo& layer, float deltaTime, Motion* motion)
{
	if (layer.blendWeight < kBlendTime)
		layer.blendWeight += deltaTime;

	layer.currentMotionTime += deltaTime;
	if (layer.currentMotionTime >= layer.maxFrameTime)
	{
		if (layer.isLooping == false)
			convertLayerState(layer, AnimationState::IDLE, motion);
		layer.currentMotionTime = 0.0f;
	}
}

#include <iostream>
void	Animator::updateAnimationLayerListState(AnimationState state, float deltaTime)
{
	//Upper Body Layer Update
	updateLayerState(_animationLayerList[1], deltaTime, _motionList[static_cast<int32>(state)]);
	if ((_animationLayerList[0].currentState == AnimationState::FORWARD || _animationLayerList[0].currentState == AnimationState::BACKWARD || _animationLayerList[0].currentState == AnimationState::RUN) && (state == AnimationState::ATTACK))
		convertLayerState(_animationLayerList[1], state, _motionList[static_cast<int32>(state)]);

	//Whole Body Layer Update
	updateLayerState(_animationLayerList[0], deltaTime, _motionList[static_cast<int32>(state)]);
	if (_animationLayerList[0].currentState != state && _animationLayerList[1].currentState != AnimationState::ATTACK)
		convertLayerState(_animationLayerList[0], state, _motionList[static_cast<int32>(state)]);

}

#define LOWER_BACK 10

glm::quat Animator::getJointAnimation(int32 jointIndex)
{
	glm::quat jointAnimation;

	if (jointIndex >= LOWER_BACK && _animationLayerList[1].currentState == AnimationState::ATTACK)
	{
		int32 motionIndex = static_cast<int32>(_animationLayerList[1].currentState);
		jointAnimation = _motionList[motionIndex]->getJointPose(jointIndex, _animationLayerList[1].currentMotionTime);
		if (_animationLayerList[1].blendWeight < kBlendTime)
		{
			int32 prevMotionIndex = static_cast<int32>(_animationLayerList[1].prevState);
			glm::quat prevJointAnimation = _motionList[prevMotionIndex]->getJointPose(jointIndex, _animationLayerList[1].prevMotionTime);
			jointAnimation = glm::slerp(prevJointAnimation, jointAnimation, _animationLayerList[1].blendWeight / kBlendTime);
		}
	}
	else
	{
		int32 motionIndex = static_cast<int32>(_animationLayerList[0].currentState);
		jointAnimation = _motionList[motionIndex]->getJointPose(jointIndex, _animationLayerList[0].currentMotionTime);
		if (_animationLayerList[0].blendWeight < kBlendTime)
		{
			int32 prevMotionIndex = static_cast<int32>(_animationLayerList[0].prevState);
			glm::quat prevJointAnimation = _motionList[prevMotionIndex]->getJointPose(jointIndex, _animationLayerList[0].prevMotionTime);
			jointAnimation = glm::slerp(prevJointAnimation, jointAnimation, _animationLayerList[0].blendWeight / kBlendTime);
		}
	}

	return jointAnimation;
}
