# 3D Character Animation Engine

![temp](https://github.com/Icefin/CharacterEngine/assets/76864202/73a4a525-380e-4221-b38f-687b38c4e773)

https://github.com/Icefin/CharacterEngine/assets/76864202/f0e61863-dc8a-45bb-bc19-8b3b037336ec

3D Character Animation을 위한 C++/OpenGL 프로젝트입니다.  
Character Skeleton Data로 .asf 형식을 사용하며  
Character Animation Data로 .amc 형식을 사용합니다.  
[.asf/.amc file format](http://graphics.cs.cmu.edu/nsp/course/cs229/info/Acclaim_Skeleton_Format.html)

프로젝트의 기능은
1) Animation Compression
2) Motion Blending
3) Cloth Simulation

의 세 파트로 구성되어 있습니다.  

각 파트의 요약과 목차는 아래와 같습니다.  
링크를 통해 해당 내용으로 이동할 수 있습니다.  

##### [Animation_Compression](#animation-compression)  
1. Quaternion -> Quantized Quaternion으로 자료형을 압축.  
 -> 128bit의 자료형이 48bit으로 압축되어, 0.375의 압축률
2. Curve Fitting을 통해 모션 데이터를 압축.  
 -> Threshold 0.01을 기준으로 약 1000 프레임의 애니메이션이 460프레임으로 압축되어, 0.46의 압축률
   
##### [Motion_Blending](#motion-blending)  
1. Animation Transition을 통해 부드러운 모션 전환 구현
2. Animation Layering을 통해 상/하체가 분리된 모션 구현

##### [Cloth_Simulation](#cloth-simulation)
1. Collision Detection을 위해 Sphere/OBB/AABB의 Collider 구현
2. Position Based Dynamics를 사용하여 보다 안정적인 Cloth Simulation 구현 


---
## Animation Compression

#### Quaternion Quantization
Riot Games의 Compressing Skeletal Animation Data 문서를 참고하여 QuantizedQuaternion 자료형을 결정하였습니다. 
![image](https://github.com/Icefin/3DCharacterAnimEngine/assets/76864202/12976b8b-c167-4952-8acd-1c34ad815409)
```c++
struct QuantizedQuaternion
{
    uint16 largest    : 2;  // The largest component of the quaternion.
    uint16 a          : 14;
    uint16 sign_a     : 1;
    uint16 sign_b     : 1;
    uint16 b          : 14;
    uint16 sign_c     : 1;
    uint16 sign       : 1;  // The sign of the largest component. 1 for negative.
    uint16 c          : 14;
};
```
glm::quat을 위의 자료형으로 압축하는 방법은 아래와 같습니다.
1. glm::quat에서 몇 번째 원소의 절댓값이 가장 큰지 확인
2. 해당 원소의 index를 largest에 저장 (0 ~ 3이므로 2bit로 저장 가능), 부호를 sign에 저장
3. 가장 큰 원소를 제외한 나머지 원소들을 14bit범위의 양의 정수형태로 변환하여 a, b, c 에 저장
4. 나머지 원소들의 부호를 각각 sign_a, sign_b, sign_c 변수에 저장
```c++
QuantizedQuaternion quantizeQuaternion(const glm::quat quaternion)
{
	QuantizedQuaternion quantizedQuaternion;
    	float quat[4] = { quaternion.w, quaternion.x, quaternion.y, quaternion.z };

    	//가장 큰 원소 탐색
	for (uint8 idx = 1; idx < 4; ++idx)
    	{
        	if (std::abs(quat[idx]) > maxValue)
        	{
			maxValue = std::abs(quat[idx]);
            		maxIndex = idx;
        	}
    	}
	//가장 큰 원소의 index와 부호를 3bit에 저장
	quantizedQuaternion.largest = maxIndex;
	quantizedQuaternion.sign = static_cast<uint16>(quat[maxIndex] < 0.f ? 1 : 0);

	//가장 큰 원소를 제외한 나머지 원소들에서 sqrt(2)보다 큰 값은 나올 수 없으므로
	//정밀도를 높이기 위해 16383에 sqrt(2)를 곱한 값을 사용하여 변환.
	const static float Float2Int = 16383.f * sqrtf(2);
	const int permutation[4][3] = { {1, 2, 3}, {0, 2, 3}, {0, 1, 3}, {0, 1, 2} };
	const int* map = permutation[maxIndex];

	//가장 큰 원소를 제외한 나머지 원소들을 14bit 범위의 정수로 변환
	const int16 a = static_cast<int16>(quat[map[0]] * Float2Int);
	const int16 b = static_cast<int16>(quat[map[1]] * Float2Int);
	const int16 c = static_cast<int16>(quat[map[2]] * Float2Int);

	//가장 큰 원소를 제외한 나머지 원소들의 부호를 1bit에 저장
	quantizedQuaternion.sign_a = static_cast<uint16>(a & 0x8000 ? 1 : 0);
	quantizedQuaternion.sign_b = static_cast<uint16>(b & 0x8000 ? 1 : 0);
	quantizedQuaternion.sign_c = static_cast<uint16>(c & 0x8000 ? 1 : 0);

	quantizedQuaternion.a = (quantizedQuaternion.sign_a == 0) ? static_cast<uint16>(a & 0x3FFF) : static_cast<uint16>(-a & 0x3FFF);
	quantizedQuaternion.b = (quantizedQuaternion.sign_b == 0) ? static_cast<uint16>(b & 0x3FFF) : static_cast<uint16>(-b & 0x3FFF);
	quantizedQuaternion.c = (quantizedQuaternion.sign_c == 0) ? static_cast<uint16>(c & 0x3FFF) : static_cast<uint16>(-c & 0x3FFF);

	return quantizedQuaternion;
}
```
이를 통해 기존에 x, y, z, w 4개의 float을 사용하던 glm::quat 자료형을 48bit의 새로운 자료형으로 변환합니다.  
이는 128bit 크기의 자료형을 48bit의 자료형으로 압축하므로, 0.375의 압축률을 가집니다.  
  
#### Curve Fitting
Curve Fitting을 위해 Catmull-Rom Spline을 사용합니다.  
Catmull-Rom Spline은 보간 시 곡선이 control point에 얼마나 강하게 결합되는지 결정하는 tension값 (0 ~ 1) 에 따라 곡선의 형태가 달라지는데, 현재 프로젝트에서는 0.5의 값을 사용합니다.  
![image](https://github.com/Icefin/3DCharacterAnimEngine/assets/76864202/d566c16d-8d97-41b4-931f-5b018963d896)

```c++
//catmull-rom spline with tension = 0.5
float interpolateCatmullRomSpline(float p0, float p1, float p2, float p3, float t)
{
	float c0 = 0.5f * 2 * p1;
	float c1 = 0.5f * t * (-p0 + p2);
	float c2 = 0.5f * t * t * (2 * p0 - 5 * p1 + 4 * p2 - p3);
	float c3 = 0.5f * t * t * t * (-p0 + 3 * p1 - 3 * p2 + p3);
	return (c0 + c1 + c2 + c3);
}
```

이제 Keyframe을 4개씩 뽑아서 Catmull-Rom Spline을 계산하고 보간된 데이터와 원본 데이터를 비교하여,  
1) Spline이 threshold 이하로 근사되지 않는다면 오차 범위가 가장 컸던 범위에서 새로운 keyframe을 설정 후 압축을 다시 진행합니다.  
2) Spline이 threshold 이하로 근사된다면 압축을 멈추고, 선별된 keyframe데이터들을 Quantization 후 반환합니다.  

![curve_fitting](https://github.com/Icefin/3DCharacterAnimEngine/assets/76864202/b3730a20-ec71-4cc3-8547-14442c0535e3)

```c++
std::vector<CompressedAnimationData>	CharacterLoader::compressAnimation(std::vector<AnimationData>& data)
{
	static constexpr float kThreshold = 0.01f;

	int32 dataSize = data.size();
	std::vector<std::pair<int32, glm::quat>> keyFrameRotation(dataSize);

	//Catmull-Rom Spline을 그리기 위해 keypoint가 최소 4개 필요하므로
	//데이터의 초기값과 마지막값을 더미데이터로 삽입.
	keyFrameRotation[0] = { 0, data[0].rotation };
	keyFrameRotation[1] = { 0, data[0].rotation };
	keyFrameRotation[2] = { dataSize / 2, data[dataSize / 2].rotation };
	keyFrameRotation[3] = { dataSize - 1, data[dataSize - 1].rotation };
	keyFrameRotation[4] = { dataSize - 1, data[dataSize - 1].rotation };
	int32 keyFrameSize = 5;

	//모든 구간에서 오차가 threshold보다 작아질 때 까지 압축을 진행.
	bool isCompressed = false;
	while (isCompressed == false)
	{
		float maxOffset = 0.0f;
		int32 maxRangeIndex = 0;

		//압축된 keyframe으로 Spline을 계산하며
		for (int32 i = 0; i < keyFrameSize - 3; ++i)
		{
			glm::quat p0 = keyFrameRotation[i].second;
			glm::quat p1 = keyFrameRotation[i + 1].second;
			glm::quat p2 = keyFrameRotation[i + 2].second;
			glm::quat p3 = keyFrameRotation[i + 3].second;

			int32 startFrame = keyFrameRotation[i + 1].first;
			int32 endFrame = keyFrameRotation[i + 2].first;
			int32 frameRange = endFrame - startFrame;

			// 원본 데이터와 오차를 비교.
			for (int32 t = startFrame + 1; t < endFrame; ++t)
			{
				glm::quat frameQuat = data[t].rotation;
				float timeStep = (t - startFrame) / frameRange;
				float CRx = interpolateCatmullRomSpline(p0.x, p1.x, p2.x, p3.x, timeStep);
				float CRy = interpolateCatmullRomSpline(p0.y, p1.y, p2.y, p3.y, timeStep);
				float CRz = interpolateCatmullRomSpline(p0.z, p1.z, p2.z, p3.z, timeStep);
				float CRw = interpolateCatmullRomSpline(p0.w, p1.w, p2.w, p3.w, timeStep);

				float currentOffset = abs(CRx - frameQuat.x) + abs(CRy - frameQuat.y) + abs(CRz - frameQuat.z) + abs(CRw - frameQuat.w);
				if (currentOffset > maxOffset)
				{
					maxOffset = currentOffset;
					maxRangeIndex = i;
				}
			}
		}

		//모든 구간에서 가장 큰 오차가 threshold보다 작으면 압축을 완료.
		if (maxOffset < kThreshold)
		{
			isCompressed = true;
			continue;
		}

		//압축이 완료되지 않았다면, 오차가 가장 컸던 범위에서 새로운 keyframe을 설정.
		int32 targetFrame = (keyFrameRotation[maxRangeIndex + 1].first + keyFrameRotation[maxRangeIndex + 2].first) / 2;
		std::pair<int32, glm::quat> newFrame = { targetFrame, data[targetFrame].rotation };
		keyFrameRotation.insert(keyFrameRotation.begin() + maxRangeIndex + 2, newFrame);
		keyFrameSize++;
	}

	//압축된 keyframe 데이터를 Quantization하여 최종적으로 저장.
	std::vector<CompressedAnimationData> compressedAnimation(keyFrameSize);
	for (int32 i = 0; i < keyFrameSize; ++i)
	{
		compressedAnimation[i].keyTime = keyFrameRotation[i].first;
		compressedAnimation[i].rotation = quantizeQuaternion(keyFrameRotation[i].second);
	}
	return compressedAnimation;
}
```

#### Results:
##### Threshold 0.01f :: Average 1086 frames per joint -> 457 frames per joint (0.42 compression ratio)
https://github.com/Icefin/CharacterEngine/assets/76864202/f53cf788-5df6-495c-bfa5-d5ccb894ec4a

##### Threshold 0.1f  :: Average 1086 frames per joint -> 105 frames per joint (0.1 compression ratio)
https://github.com/Icefin/CharacterEngine/assets/76864202/50d63c9d-025d-4fbc-8035-72719ec1158b

#### Reference :  
https://technology.riotgames.com/news/compressing-skeletal-animation-data  
https://www.cs.cmu.edu/~fp/courses/graphics/asst5/catmullRom.pdf  
https://splines.readthedocs.io/en/latest/euclidean/catmull-rom-barry-goldman.html  

---
## Motion Blending

#### Animation Transition

Animation Transition을 통해 하나의 모션에서 다른 모션으로 자연스럽게 전환되도록 구현합니다.  
Animator가 Animation들의 상태를 관리하며, 현재 진행되고 있는 Animation의 weight factor는 1의 값을 가지게 됩니다.  
이후 사용자의 입력을 통해 Animation의 상태가 전환되면 30ms에 걸쳐 현재 진행되고 있는 Animation의 weight factor는 0으로, 새로 진행해야 하는 Animation의 weight factor 는 1의 값으로 갱신됩니다.  
Animator는 각 joint의 Animation 데이터 반환 시 각 Animation들의 weight factor값으로 데이터를 선형 보간하여 최종 결과물을 반환합니다.  

```c++
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

class Animator
{
public :
			Animator(std::vector<Motion*>& motionList);
			~Animator();

	void		update(AnimationState state, float deltaTime);

	glm::quat	getJointAnimation(int32 jointIndex);

private :
	void			updateAnimationLayerListState(AnimationState state, float deltaTime);
	std::vector<Motion*>	_motionList;
	std::vector<LayerInfo>	_animationLayerList;
};
```

##### Before Motion Blending
https://github.com/Icefin/CharacterEngine/assets/76864202/f7bba69e-c020-42fe-94aa-38eb51a85f8c

##### After Motion Blending
https://github.com/Icefin/CharacterEngine/assets/76864202/68c6a325-3091-4e15-932c-9c320aeeba17


#### Animation Layering

Animation Layering을 통해 상체와 하체의 애니메이션을 분리합니다.  
이를 통해 걷기/뛰기 모션 도중 공격이 입력되어도 하체는 걷는 모션을 지속하고, 상체만 공격 모션이 나갈 수 있도록 개선하였습니다.  

```c++
struct LayerInfo
{
	int32		parentLayerIndex = -1;

	int32		animationRootBoneIndex = 0;

	AnimationState	prevState = AnimationState::IDLE;
	float		prevMotionTime = 0.0f;

	AnimationState	currentState = AnimationState::IDLE;
	float		currentMotionTime = 0.0f;

	float		layerBlendWeight = kBlendTime;
	float		crossFadeBlendWeight = kBlendTime;
	int32		maxFrameTime = 0;
	bool		isLooping = true;
};
```

Full Body Animation이 가장 기본값 이므로 _animationLayerList[0]에 전신 Animation LayerInfo가 위치합니다.  
현재 프로젝트에서는 상체와 하체만 분리하므로 _animationLayerList[1]에 상체 Animation LayerInfo가 위치하게 됩니다.  
_animationLayerList[0]의 animationRootBoneIndex = 0, _animationLayerList[1]의 animationRootBoneIndex = 10의 값을 가지게 되는데, 이는 각각 character skeleton의 구조 상, 전신의 root bone index = 1, 상체의 root bone index = 10이기 때문입니다.  

Child Layer는 Parent Layer보다 높은 우선순위를 가지며, 동시에 입력될 경우 child의 animation을 반환하도록 합니다.  
현재 프로젝트에서는 상체의 animation state이 ATTACK인 경우에만 상/하체를 분리할 지 결정합니다.  
또한 상체 모션이 끝난 후, 전신의 모션과 blending 하는 것으로 기존 모션과 자연스럽게 연결될 수 있도록 하였습니다.

```c++
glm::quat Animator::getJointAnimation(int32 jointIndex)
{
	glm::quat jointAnimation;

	//상체 모션이 ATTACK인 경우
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
	//상체 모션이 ATTACK은 아니지만, layer motion blending을 해야하는 경우
	else if (jointIndex >= LOWER_BACK && _animationLayerList[1].layerBlendWeight > 0.0f)
	{
		int32 motionIndex = static_cast<int32>(_animationLayerList[0].currentState);
		jointAnimation = _motionList[motionIndex]->getJointPose(jointIndex, _animationLayerList[0].currentMotionTime);

		int32 prevMotionIndex = static_cast<int32>(_animationLayerList[1].prevState);
		glm::quat prevJointAnimation = _motionList[prevMotionIndex]->getJointPose(jointIndex, _animationLayerList[1].prevMotionTime);
		jointAnimation = glm::slerp(prevJointAnimation, jointAnimation, 1.0f - _animationLayerList[1].layerBlendWeight / kBlendTime);
	}
	//이외에 Full body animation 진행
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

```

##### After Animation Layering
https://github.com/Icefin/CharacterEngine/assets/76864202/5a8f4fbf-35e1-449a-a95d-4300f9d409c1

#### References :
https://graphics.cs.wisc.edu/Papers/2003/KG03/regCurves.pdf  
https://www.gamedeveloper.com/design/third-person-camera-view-in-games---a-record-of-the-most-common-problems-in-modern-games-solutions-taken-from-new-and-retro-games  
http://number-none.com/product/Understanding%20Slerp,%20Then%20Not%20Using%20It/   

---
## Cloth Simulation

#### Object Setting

Cloth Simulation을 위해 Cloth를 grid 형태의 질점으로 분할하고, 해당 질점들의 위치를 제한하기 위한 constraint를 다음과 같이 설정합니다.  
현재 프로젝트에서는 distance, collision constraint만을 사용합니다.  

```c++
struct MassPoint
{
	float		invMass;
	glm::vec3	prevPosition;
	glm::vec3	position;
	glm::vec3	normal;
	glm::vec3	velocity;
	glm::vec3	color;
};

struct CollisionConstraint
{
	glm::vec3	targetPosition;
	MassPoint*	point;
};

struct DistanceConstraint
{
	float		restLength;
	MassPoint*	left;
	MassPoint*	right;
};
```

Cloth 객체는 생성 시 widthNum * heightNum의 grid형태의 massPoint로 분할되며, 이 때 각 massPoint는 Structural, Shear, Bend constraint로 연결됩니다.  
Mass-Spring System에서는 각각의 spring의 stiffness를 설정해야 하지만, 현재 프로젝트에서는 모두 distance constraint의 형태로 _internalConstraints에 저장합니다.  
![image](https://github.com/Icefin/3DCharacterAnimEngine/assets/76864202/953dcac3-c839-48cd-a488-9a9c2d4c086c)

이후 매 프레임 update함수를 호출하며, Position Based Dynamics의 알고리즘을 따라 상태를 갱신합니다.  
1) massPoint의 초기값을 설정.
2) 충돌을 검사하고, 충돌이 발생 시 collision constraint를 생성.
3) 생성된 constraint을 해소하며 massPoint의 위치를 갱신.
4) 최종적으로 결정된 massPoint의 위치를 이용하여 속도를 갱신.

```c++
class PlaneCloth : public GameObject
{
public:
	PlaneCloth(glm::vec3 position, glm::vec3 color, uint32 width, uint32 height, uint32 widthNum, uint32 heightNum);
	~PlaneCloth(void);

	void update(float deltaTime, std::vector<pa::OBB>& colliders);
	void render(Shader& shader);

private:
	void generateCollisionConstraint(MassPoint& massPoint, std::vector<pa::OBB> colliders, std::vector<CollisionConstraint>* collisionConstraints);
	void solveDistanceConstraint(DistanceConstraint& constraint);
	void solveCollisionConstraint(CollisionConstraint& constraint);
	void updateMassPointNormal(void);

private:
	GLuint	_vao;
	GLuint	_vbo;
	GLuint	_ebo;

	std::vector<MassPoint>		_massPointList;
	std::vector<uint32>		_indices;
	std::vector<DistanceConstraint> _internalConstraints;

private:
	glm::vec3	_materialAmbient{0.1f, 0.1f, 0.1f};
	glm::vec3	_materialSpecular{0.2f, 0.2f, 0.2f};
	glm::vec3	_materialEmissive{0.0f, 0.0f, 0.0f};
	float		_materialShininess = 5.0f;
};
```

#### Position Based Dynamics

```c++
void PlaneCloth::update(float deltaTime, std::vector<pa::OBB>& colliders)
{
	static int32 kIterationCount = 5;

	//초기값 설정
	for (MassPoint& massPoint : _massPointList)
	{
		massPoint.velocity = massPoint.velocity + pa::gravity * massPoint.invMass * deltaTime;
		massPoint.prevPosition = massPoint.position;
		massPoint.position = massPoint.position + massPoint.velocity * deltaTime;
		massPoint.color = glm::vec3(0.9f, 0.9f, 0.9f);
	}

	//충돌을 확인하고, 충돌이 발생하면 collisionConstraint를 생성.
	std::vector<CollisionConstraint> collisionConstraints;
	collisionConstraints.reserve(_massPointList.size());
	for (MassPoint& massPoint : _massPointList)
		generateCollisionConstraint(massPoint, colliders, &collisionConstraints);

	//생성된 모든 constraint를 iterationCount만큼 반복하며 massPoint의 위치를 결정.
	for (int32 cnt = 0; cnt < kIterationCount; ++cnt)
	{
		for (DistanceConstraint& constraint : _internalConstraints)
			solveDistanceConstraint(constraint);

		for (CollisionConstraint& constraint : collisionConstraints)
			solveCollisionConstraint(constraint);
	}

	//최종적으로 결정된 위치를 이용하여 massPoint의 속도를 결정.
	for (MassPoint& massPoint : _massPointList)
		massPoint.velocity = 0.99f * (massPoint.position - massPoint.prevPosition) / deltaTime;

	updateMassPointNormal();
}
```

generateCollisionConstraint함수에서는 Scene에 존재하는 모든 OBB와 massPoint의 충돌을 검사합니다.
massPoint가 OBB의 내부에 존재하면 충돌이 발생한 상황 이므로 충돌을 해소할 수 있는 위치로 massPoint를 밀어내는 constraint를 생성합니다.  
```c++
void PlaneCloth::generateCollisionConstraint(MassPoint& massPoint, std::vector<pa::OBB> colliders, std::vector<CollisionConstraint>* collisionConstraints)
{
	for (pa::OBB& obb : colliders)
	{
		if (pa::isPointInside(massPoint.position, obb) == true)
		{
			massPoint.color = glm::vec3(1.0f, 0.0f, 0.0f);

			glm::vec3 obbToPoint = massPoint.position - obb.position;
			glm::mat3 rotation = glm::mat3(obb.orientation);
			glm::vec3 basis[3] = {
				{rotation[0][0], rotation[0][1], rotation[0][2]},
				{rotation[1][0], rotation[1][1], rotation[1][2]},
				{rotation[2][0], rotation[2][1], rotation[2][2]}
			};

			pa::Point targetPoint;
			float minDistance = FLT_MAX;
			for (int32 i = 0; i < 3; ++i)
			{
				float distance = glm::dot(obbToPoint, basis[i]);
				if (distance > 0.0f && obb.size[i] - distance < minDistance)
				{
					minDistance = obb.size[i] - distance;
					targetPoint = massPoint.position + (minDistance + 0.01f) * basis[i];
				}
				else if (distance < 0.0f && obb.size[i] + distance < minDistance)
				{
					minDistance = obb.size[i] + distance;
					targetPoint = massPoint.position - (minDistance + 0.01f) * basis[i];
				}
			}
			
			collisionConstraints->push_back({ targetPoint, &massPoint });
			return;
		}
	}
}
```

Point가 OBB내부에 위치하는지 확인하는 함수는 아래와 같습니다.  
OBB의 중심점과 변의 길이를 사용하여 Point가 OBB의 내부에 위치하는지 검사합니다.  
```c++
bool isPointInside(const Point& point, const OBB& obb)
{
	glm::vec3 obbToPoint = point - obb.position;

	glm::mat3 rotation = glm::mat3(obb.orientation);
	for (int32 i = 0; i < 3; i++)
	{
		glm::vec3 basis = glm::normalize(glm::vec3(rotation[i][0], rotation[i][1], rotation[i][2]));
		float distance = glm::dot(obbToPoint, basis);

		if (distance > obb.size[i])
			return false;
		if (distance < -obb.size[i])
			return false;
	}
	return true;
}
```

Distance Constraint와 Collision Constraint를 해소하는데 사용한 함수는 아래와 같습니다.
```c++
void PlaneCloth::solveDistanceConstraint(DistanceConstraint& constraint)
{
	MassPoint* left = constraint.left;
	MassPoint* right = constraint.right;

	float invMassSum = left->invMass + right->invMass;

	if (invMassSum == 0.0f)
		return;

	glm::vec3 n = right->position - left->position;
	float distance = glm::length(n);

	glm::vec3 direction = glm::normalize(n);

	glm::vec3 correction = direction * (distance - constraint.restLength) / invMassSum;

	left->position += correction * left->invMass;
	right->position -= correction * right->invMass;
}

void PlaneCloth::solveCollisionConstraint(CollisionConstraint& constraint)
{
	if (constraint.point->invMass == 0.0f)
		return;

	constraint.point->position = constraint.targetPosition;
}
```

#### Result :

![res](https://github.com/Icefin/CharacterEngine/assets/76864202/8b91ed6b-2270-44bc-8cf5-fb3d95caf2bd)

##### First :: Fixed Volume + Mass-Spring System

https://github.com/Icefin/CharacterEngine/assets/76864202/b2d7e7f4-0658-4454-9d6f-6fc19bdacc7d

##### Second :: OBB Collider + Mass-Spring System

https://github.com/Icefin/CharacterEngine/assets/76864202/79cca57a-cb96-4bd2-86e4-e4db2ce76b0a

##### Final :: OBB Collider + Position Based Dynamics

https://github.com/Icefin/CharacterEngine/assets/76864202/b6a18c6c-c777-4bf8-a5d4-6b92fca674ff

Constraint - Vertex Number Relation (per frame)
|     | 10 * 10 | 20 * 20 | 30 * 30 | 40 * 40 |
|:---:|:-------:|:-------:|:-------:|:-------:|
|**1**| 0.016ms | 0.016ms | 0.027ms | 0.047ms |
|**2**| 0.016ms | 0.016ms | 0.034ms | 0.057ms |
|**3**| 0.016ms | 0.018ms | 0.038ms | 0.067ms |
|**4**| 0.016ms | 0.020ms | 0.045ms | 0.077ms |
|**5**| 0.016ms | 0.023ms | 0.050ms | 0.087ms |

Iteration Count - Vertex Number Relation (per frame)
|     | 10 * 10 | 20 * 20 | 30 * 30 | 40 * 40 |
|:---:|:-------:|:-------:|:-------:|:-------:|
|**1**| 0.016ms | 0.016ms | 0.031ms | 0.055ms |
|**2**| 0.016ms | 0.017ms | 0.036ms | 0.063ms |
|**3**| 0.016ms | 0.019ms | 0.040ms | 0.071ms |
|**4**| 0.016ms | 0.021ms | 0.045ms | 0.080ms |
|**5**| 0.016ms | 0.023ms | 0.050ms | 0.087ms |

#### Reference :
https://ics.uci.edu/~shz/courses/cs114/docs/proj3/index.html  
https://graphics.stanford.edu/~mdfisher/cloth.html  
https://carmencincotti.com/2022-07-11/position-based-dynamics/

---
