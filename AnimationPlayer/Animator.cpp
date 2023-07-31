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
	_animationLayerList[0].maxFrameTime = _motionList[0]->getMaxFrameTime();

	_animationLayerList[1].parentLayerIndex = 0;
	_animationLayerList[1].animationRootBoneIndex = 11;
	_animationLayerList[1].maxFrameTime = _motionList[0]->getMaxFrameTime();

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

	layer.crossFadeBlendWeight = kBlendTime - layer.crossFadeBlendWeight;
	layer.prevMotionTime = layer.currentMotionTime;
	layer.currentMotionTime = 0.0f;

	layer.maxFrameTime = motion->getMaxFrameTime();
	layer.isLooping = motion->_isLooping;
}

static void updateLayerState(LayerInfo& layer, AnimationState state, float deltaTime, Motion* motion)
{
	if (layer.crossFadeBlendWeight < kBlendTime)
		layer.crossFadeBlendWeight += deltaTime;

	layer.currentMotionTime += deltaTime;
	if (layer.currentMotionTime >= layer.maxFrameTime)
	{
		if (layer.isLooping == false)
			convertLayerState(layer, state, motion);
		layer.currentMotionTime = 0.0f;
	}
}

void	Animator::updateAnimationLayerListState(AnimationState state, float deltaTime)
{
	int32 layerNumber = _animationLayerList.size();
	for (int idx = 0; idx < layerNumber; ++idx)
		updateLayerState(_animationLayerList[idx], state, deltaTime, _motionList[static_cast<int32>(state)]);

	if ((_animationLayerList[0].currentState == AnimationState::FORWARD || _animationLayerList[0].currentState == AnimationState::BACKWARD || _animationLayerList[0].currentState == AnimationState::RUN))
	{
		if (_animationLayerList[1].currentState == AnimationState::IDLE && state == AnimationState::ATTACK)
		{
			_animationLayerList[1].prevState = _animationLayerList[0].currentState;
			_animationLayerList[1].currentState = AnimationState::ATTACK;

			_animationLayerList[1].crossFadeBlendWeight = kBlendTime - _animationLayerList[0].crossFadeBlendWeight;
			_animationLayerList[1].prevMotionTime = _animationLayerList[0].currentMotionTime;
			_animationLayerList[1].currentMotionTime = 0.0f;

			_animationLayerList[1].maxFrameTime = _motionList[5]->getMaxFrameTime();
			_animationLayerList[1].isLooping = _motionList[5]->_isLooping;
		}
	}

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
		if (_animationLayerList[1].crossFadeBlendWeight < kBlendTime)
		{
			int32 prevMotionIndex = static_cast<int32>(_animationLayerList[1].prevState);
			glm::quat prevJointAnimation = _motionList[prevMotionIndex]->getJointPose(jointIndex, _animationLayerList[1].prevMotionTime);
			jointAnimation = glm::slerp(prevJointAnimation, jointAnimation, _animationLayerList[1].crossFadeBlendWeight / kBlendTime);
		}
	}
	else
	{
		int32 motionIndex = static_cast<int32>(_animationLayerList[0].currentState);
		jointAnimation = _motionList[motionIndex]->getJointPose(jointIndex, _animationLayerList[0].currentMotionTime);
		if (_animationLayerList[0].crossFadeBlendWeight < kBlendTime)
		{
			int32 prevMotionIndex = static_cast<int32>(_animationLayerList[0].prevState);
			glm::quat prevJointAnimation = _motionList[prevMotionIndex]->getJointPose(jointIndex, _animationLayerList[0].prevMotionTime);
			jointAnimation = glm::slerp(prevJointAnimation, jointAnimation, _animationLayerList[0].crossFadeBlendWeight / kBlendTime);
		}
	}

	return glm::normalize(jointAnimation);
}
