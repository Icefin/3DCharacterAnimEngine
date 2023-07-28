#include "Animator.h"

Animator::Animator()
{
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

glm::quat Animator::getBoneAnimation(int32 boneIndex)
{
	//find bone's animation layer

	//return animation with layerinfo + boneindex
	return glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
}
