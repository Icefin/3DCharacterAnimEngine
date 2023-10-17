# 3D Character Animation Engine

![temp](https://github.com/Icefin/CharacterEngine/assets/76864202/73a4a525-380e-4221-b38f-687b38c4e773)

https://github.com/Icefin/CharacterEngine/assets/76864202/f0e61863-dc8a-45bb-bc19-8b3b037336ec

//전체 프로젝트 설명

##### [Animation Compression](#animation-compression)  
1. Quaternion -> Quantized Quaternion으로 자료형을 압축.  
 -> 128bit (x, y, z, w의 4float) -> 48bit으로 변경되며, 0.375의 압축률
2. Catmull-Rom Spline을 사용하여 모션 데이터를 압축.  
 -> Threshold 0.01을 기준으로 약 1000 프레임의 애니메이션이 460프레임으로 압축되어, 0.46의 압축률
   
##### [Motion Blending](#motion-blending)  
1. Animation Transition을 통해 부드러운 모션 전환 구현
2. Animation Layering을 통해 상/하체가 분리된 모션 구현

##### [Cloth Simulation](#cloth-simulation)
1. Collision Detection을 위해 Sphere/OBB/AABB의 Collider 구현
2. 옷감의 음영을 확인하기 위해 Phong Light Model을 사용하여 Lighting
3. Position Based Dynamics를 사용하여 보다 안정적인 Cloth Simulation 구현 


---
## Animation Compression

#### Quaternion Quantization
// Quantization 설명
  
#### Catmull-Rom Interpolation with u = 0.5
//catmull-rom spline을 어떻게 적용하는지 설명
```c++
float interpolateCatmullRomSpline(float p0, float p1, float p2, float p3, float t)
{
	float c0 = 0.5f * 2 * p1;
	float c1 = 0.5f * t * (-p0 + p2);
	float c2 = 0.5f * t * t * (2 * p0 - 5 * p1 + 4 * p2 - p3);
	float c3 = 0.5f * t * t * t * (-p0 + 3 * p1 - 3 * p2 + p3);
	return (c0 + c1 + c2 + c3);
}
```

#### Animation Data Compression
//데이터 압축되는 로직 설명
```c++
std::vector<CompressedAnimationData>	CharacterLoader::compressAnimation(std::vector<AnimationData>& data)
{
	static constexpr float kThreshold = 0.01f;

	//Already know maximum size
	int32 dataSize = data.size();
	std::vector<std::pair<int32, glm::quat>> keyFrameRotation(dataSize);

	//Insert Dummy Data to both ends
	keyFrameRotation[0] = { 0, data[0].rotation };
	keyFrameRotation[1] = { 0, data[0].rotation };
	keyFrameRotation[2] = { dataSize / 2, data[dataSize / 2].rotation };
	keyFrameRotation[3] = { dataSize - 1, data[dataSize - 1].rotation };
	keyFrameRotation[4] = { dataSize - 1, data[dataSize - 1].rotation };
	int32 keyFrameSize = 5;

	//Compression Start
	bool isCompressed = false;
	while (isCompressed == false)
	{
		float maxOffset = 0.0f;
		int32 maxRangeIndex = 0;

		//Threshold check
		for (int32 i = 0; i < keyFrameSize - 3; ++i)
		{
			glm::quat p0 = keyFrameRotation[i].second;
			glm::quat p1 = keyFrameRotation[i + 1].second;
			glm::quat p2 = keyFrameRotation[i + 2].second;
			glm::quat p3 = keyFrameRotation[i + 3].second;

			int32 startFrame = keyFrameRotation[i + 1].first;
			int32 endFrame = keyFrameRotation[i + 2].first;
			int32 frameRange = endFrame - startFrame;

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

		if (maxOffset < kThreshold)
		{
			isCompressed = true;
			continue;
		}

		//If is not compressed enough, insert new key frame
		int32 targetFrame = (keyFrameRotation[maxRangeIndex + 1].first + keyFrameRotation[maxRangeIndex + 2].first) / 2;
		std::pair<int32, glm::quat> newFrame = { targetFrame, data[targetFrame].rotation };
		keyFrameRotation.insert(keyFrameRotation.begin() + maxRangeIndex + 2, newFrame);
		keyFrameSize++;
	}

	//Generate Compressed Animation Data
	std::vector<CompressedAnimationData> compressedAnimation(keyFrameSize);
	for (int32 i = 0; i < keyFrameSize; ++i)
	{
		compressedAnimation[i].keyTime = keyFrameRotation[i].first;
		compressedAnimation[i].rotation = quantizeQuaternion(keyFrameRotation[i].second);
	}
	return compressedAnimation;
}
```

#### Decompress & Get Animation Data
//데이터 압축해제 설명
```c++
glm::quat	Motion::getBoneAnimation(int32 boneIndex)
{
	std::vector<CompressedAnimationData>& boneAnimation = _keyFrameAnimations[boneIndex];
	if (boneAnimation.empty() == true)
		return glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

	//Binary Search to find target index
	int32 targetFrameIndex = 1;
	int32 lowerFrameIndex = 1, upperFrameIndex = boneAnimation.size() - 3;
	while (lowerFrameIndex <= upperFrameIndex)
	{
		int32 midFrameIndex = lowerFrameIndex + (upperFrameIndex - lowerFrameIndex) / 2;

		if (boneAnimation[midFrameIndex].keyTime <= _keyFrameTime)
		{
			targetFrameIndex = midFrameIndex;
			lowerFrameIndex = midFrameIndex + 1;
		}
		else
			upperFrameIndex = midFrameIndex - 1;
	}

	//DequnatizeQuaternion
	glm::quat p0 = dequantizeQuaternion(boneAnimation[targetFrameIndex - 1].rotation);
	glm::quat p1 = dequantizeQuaternion(boneAnimation[targetFrameIndex].rotation);
	glm::quat p2 = dequantizeQuaternion(boneAnimation[targetFrameIndex + 1].rotation);
	glm::quat p3 = dequantizeQuaternion(boneAnimation[targetFrameIndex + 2].rotation);
	
	//Interpolate Catmull-Rom
	int32 startKeyTime = boneAnimation[targetFrameIndex].keyTime;
	int32 endKeyTime = boneAnimation[targetFrameIndex + 1].keyTime;
	
	float t = (_keyFrameTime - startKeyTime) / (endKeyTime - startKeyTime);
	float x = interpolateCatmullRomSpline(p0.x, p1.x, p2.x, p3.x, t);
	float y = interpolateCatmullRomSpline(p0.y, p1.y, p2.y, p3.y, t);
	float z = interpolateCatmullRomSpline(p0.z, p1.z, p2.z, p3.z, t);
	float w = interpolateCatmullRomSpline(p0.w, p1.w, p2.w, p3.w, t);

	//Return normalized quaternion
	return glm::normalize(glm::quat(w, x, y, z));
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

//한글 번역
A transition involves two motions and a weight function that
starts at (1,0) and smoothly changes to (0,1), and an interpolation combines an arbitrary number of motions according
to a constant weight function.

//animation transition 로직?

##### Before Motion Blending
https://github.com/Icefin/CharacterEngine/assets/76864202/f7bba69e-c020-42fe-94aa-38eb51a85f8c

##### After Motion Blending
https://github.com/Icefin/CharacterEngine/assets/76864202/68c6a325-3091-4e15-932c-9c320aeeba17


#### Animation Layering
//animation layering 설명
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
```
##### After Animation Layering
https://github.com/Icefin/CharacterEngine/assets/76864202/5a8f4fbf-35e1-449a-a95d-4300f9d409c1

#### References :
https://graphics.cs.wisc.edu/Papers/2003/KG03/regCurves.pdf  
https://www.gamedeveloper.com/design/third-person-camera-view-in-games---a-record-of-the-most-common-problems-in-modern-games-solutions-taken-from-new-and-retro-games   
http://number-none.com/product/Understanding%20Slerp,%20Then%20Not%20Using%20It/   

---
## Cloth Simulation

#### Cloth class with Constraint
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

#### Position Based Dynamics with Collision
![pbd_algo](https://github.com/Icefin/CharacterEngine/assets/76864202/7dad8ab8-3eaf-453b-ba90-9fe850963a9b)


```c++
void PlaneCloth::update(float deltaTime, std::vector<pa::OBB>& colliders)
{
	static int32 kIterationCount = 5;

	//Store Initial Values by using Sympletic Euler integration
	for (MassPoint& massPoint : _massPointList)
	{
		massPoint.velocity = massPoint.velocity + pa::gravity * massPoint.invMass * deltaTime;
		massPoint.prevPosition = massPoint.position;
		massPoint.position = massPoint.position + massPoint.velocity * deltaTime;
		massPoint.color = glm::vec3(0.9f, 0.9f, 0.9f);
	}

	//Collision Detection && Generate Collision Constraint
	std::vector<CollisionConstraint> collisionConstraints;
	collisionConstraints.reserve(_massPointList.size());
	for (MassPoint& massPoint : _massPointList)
		generateCollisionConstraint(massPoint, colliders, &collisionConstraints);

	//Solve Constraints
	for (int32 cnt = 0; cnt < kIterationCount; ++cnt)
	{
		for (DistanceConstraint& constraint : _internalConstraints)
			solveDistanceConstraint(constraint);

		for (CollisionConstraint& constraint : collisionConstraints)
			solveCollisionConstraint(constraint);
	}

	//Velocity Update
	for (MassPoint& massPoint : _massPointList)
		massPoint.velocity = 0.99f * (massPoint.position - massPoint.prevPosition) / deltaTime;

	updateMassPointNormal();
}
```

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
##### First try with Fixed Volume + Force based Explicit Euler Method

https://github.com/Icefin/CharacterEngine/assets/76864202/b2d7e7f4-0658-4454-9d6f-6fc19bdacc7d

##### Second try with Collision Detection + Force based Verlet Integration

https://github.com/Icefin/CharacterEngine/assets/76864202/79cca57a-cb96-4bd2-86e4-e4db2ce76b0a

##### Third try with Collision Detection + Position Based Dynamics

https://github.com/Icefin/CharacterEngine/assets/76864202/b6a18c6c-c777-4bf8-a5d4-6b92fca674ff


#### Optimization
![res](https://github.com/Icefin/CharacterEngine/assets/76864202/8b91ed6b-2270-44bc-8cf5-fb3d95caf2bd)

Constraint - Vertex Number Relation (Force based Simulation)
|     | 10 * 10 | 20 * 20 | 30 * 30 | 40 * 40 |
|:---:|:-------:|:-------:|:-------:|:-------:|
|**1**| 0.016ms | 0.016ms | 0.027ms | 0.047ms |
|**2**| 0.016ms | 0.016ms | 0.034ms | 0.057ms |
|**3**| 0.016ms | 0.018ms | 0.038ms | 0.067ms |
|**4**| 0.016ms | 0.020ms | 0.045ms | 0.077ms |
|**5**| 0.016ms | 0.023ms | 0.050ms | 0.087ms |

Iteration Count - Vertex Number Relation (Position based Simulation, 3 constraints)
|     | 10 * 10 | 20 * 20 | 30 * 30 | 40 * 40 |
|:---:|:-------:|:-------:|:-------:|:-------:|
|**1**| 0.016ms | 0.016ms | 0.031ms | 0.055ms |
|**2**| 0.016ms | 0.017ms | 0.036ms | 0.063ms |
|**3**| 0.016ms | 0.019ms | 0.040ms | 0.071ms |
|**4**| 0.016ms | 0.021ms | 0.045ms | 0.080ms |
|**5**| 0.016ms | 0.023ms | 0.050ms | 0.087ms |

1. Vertex Number Effect
- Jacobi-Iteration rather than Gauss-Seidel Method
- Parallelize Normal Calculation
2. Constraint Number Effect
- BVH for character mesh
3. Iteration Count Effect
- XPBD?

#### Reference :
https://graphics.stanford.edu/~mdfisher/cloth.html  
https://learnopengl.com/Lighting/Basic-Lighting  
https://carmencincotti.com/2022-07-11/position-based-dynamics/
