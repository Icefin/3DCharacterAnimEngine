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
}

static void updateLayerState(LayerInfo& layer, float deltaTime, Motion* motion)
{
	if (layer.blendWeight < kBlendTime)
		layer.blendWeight += deltaTime;

	layer.currentMotionTime += deltaTime;
	if (layer.currentMotionTime >= layer.maxFrameTime)
	{
		layer.currentMotionTime = 0.0f;
		if (layer.isLooping == false)
			convertLayerState(layer, AnimationState::IDLE, motion);
	}
}

void	Animator::updateAnimationLayerListState(AnimationState state, float deltaTime)
{
	int32 layerNumber = _animationLayerList.size();
	for (int32 idx = 0; idx < layerNumber; ++idx)
	{
		if (_animationLayerList[idx].currentState == state)
			updateLayerState(_animationLayerList[idx], deltaTime, _motionList[static_cast<int32>(state)]);
		else
			convertLayerState(_animationLayerList[idx], state, _motionList[static_cast<int32>(state)]);
	}
}

#define LOWER_BACK 10

glm::quat Animator::getJointAnimation(int32 jointIndex)
{
	glm::quat jointAnimation;

	if (jointIndex > LOWER_BACK && _animationLayerList[1].currentState != AnimationState::IDLE)
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
