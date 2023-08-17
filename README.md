# AnimationPlayer
Simple Character Animation Player

---
### Parsing ASF / AMC Data
#### Reference :
https://research.cs.wisc.edu/graphics/Courses/cs-838-1999/Jeff/ASF-AMC.html  
http://www.cs.cmu.edu/~kiranb/animation/StartupCodeDescription.htm  
http://graphics.cs.cmu.edu/nsp/course/cs229/info/Acclaim_Skeleton_Format.html  

#### Acclaim Skeleton File (a.k.a ASF)
Important properties : units, root, bonedata, hierarchy  
```
:units     # multiplier (optional.)
mass 1.0   # float unit system
length 1.0 # float
angle deg  # token (rad or deg)
```
The units are interpreted by multiplying the relevant data directly on reading. Ex)if the length unit s 2.54, then all incoming translation or length values should be multiplied by 2.54. This is also the case for the .amc file. Any translations or length values must be multiplied by 2.54. This allows direct scaling of the skeleton and it’s motion.  

```
:root
axis xyz                # token (rot. order for orientation offset)
order tx ty tz rz ry rx # tokens (order of transformation for root)
position 0.0 0.0 0.0.   # float (translation offset for root node)
orientation 0.0 0.0 0.0 # float (rotation offset)
```
This defines the base location and orientation of the entire skeleton. All motion is relative to this point. Typically the root is located at the origin and oriented along the z-axis.  

```
:bonedata # definition data for all the bones
begin
id 1                    # int (optional. unique numeric id)
name h_waist            # string (uses the body naming convention)
direction 0.0 1.0 0.0   # float (direction vector of bone in global space)
length 3.0              # float (length of bone)
axis 0.0 90.0 0.0 zyx   # float (global orientation of the axis specifies order of rotation)
dof tx ty tz rx ry rz l # tokens (only include tokens required)
limits (-inf inf)       # float/token 
bodymass 10.0           # float (optional. mass of skinbody assoc. with bone)
cofmass 1.0             # (optional. position of center of mass along bone)
end
begin
id 2
name l_waist
direction
…
```
This section holds all the data for each bone in global space. The data for a bone is delimited by begin/end tokens. Each tokens needs to appear in the following order. Dof specification allows for xyz translation and rotation as well as movement along the bone (”l”). This movement is translation, not scaling data and corresponds to stretching the skin. The limit information should not be used to clip data from the .amc file. The data in the .amc file has been precliped. Limits are there to aid the animator and help to define the range of motion of individual bones

```
:hierarchy
begin
root h_waist h_R_hip h_L_hip
…
```
This section defines the hierarchical relationships between the bones. The motion in the .amc file is defined in terms of this hierarchy. The data is delimited by begin/end tokens. The first entry in a line is the parent. Subsequent bones are the direct inferiors of that parent.

#### Acclaim Motion Captured Data (a.k.a AMC)
```
#Comments
...
:fully-specified
1
root 12.0 33.0 45.0 0.0 90.0 45.0
h_torso_l 0.0 0.0 0.0
h_torso_2 0.0 0.0 0.0
...
...
2
root ...
h_torso_l ...
h_torso_2 ...
...
...
```
This motion data is tied to a particular skeleton or .asf file. The format is very simple. Comments at the top followed by the format identifier. Then the data is grouped by frame. First the frame number and then each bone name followed by the values associated with each dof token in the corresponding dof line in the .asf file. The bones are in the same order for every frame.

---
### Animation Compression
#### Reference :  
https://technology.riotgames.com/news/compressing-skeletal-animation-data  
https://www.cs.cmu.edu/~fp/courses/graphics/asst5/catmullRom.pdf  
https://splines.readthedocs.io/en/latest/euclidean/catmull-rom-barry-goldman.html  
  
#### Requirements:  
1. Interpolate each Quaternion elements vs {Interpolate whole together} ?  
	Interpolating whole together is much easy and compact.  
  
2. Which data structure to use ?  
vector<pair<frame, glm::quat>> ControlPoints (circular vector lol) -> Useless....  
vector<pair<frame, glm::quat>> keyFrames (List (insertion!) vs Vector (search!))  
				List : Search : O(n), Insertion : O(1)  
				Vector : Search : O(logn), Insertion : O(n)  
   
3. Final foramt of Compressed Animation  
struct CompressedAnimation  
{  
	uint16 keyFrame;  
	QuantizedQuaternion rotation;  
}  
  
4. Do we need a new Compressor Class ?  
	I think not.... yet...?  
  
#### Final format :  
Transform.h  
float	interpolateCatmullRomSpline(float p0, float p1, float p2, float p3, float t);  
  
Motion.h  
public :  
Posture getBonePostureAtFrame(int32 boneIndex, float time);  
void	setBoneCompressedAnimation(int32 boneIndex, CompressedAnimation anim);  
  
private :  
Posture decompressAnimationData(CompressedAnimation anim);  
std::vector<std::vector<CompressedAnimation>> _keyFrameAnimations;  
  
CharacterLoader.h  
std::vector<CompressedAnimationData>	compressAnimation(std::vector<AnimationData>& data);  
  
#### Catmull-Rom Interpolation with u = 0.5
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
#### Threshold = 0.01f / Average 1086 frames per joint-> 457 frames per joint
![Animation-Player-2023-07-12-23-04-36](https://github.com/Icefin/AnimationPlayer/assets/76864202/4c3d9d69-38cb-4ba1-94f5-6236793b4b67)

#### Threshold = 0.1f  / Average 1086 frames per joint -> 105 frames per joint
![Animation-Player-2023-07-12-23-12-56](https://github.com/Icefin/AnimationPlayer/assets/76864202/716ee92f-183e-47c4-9aeb-eeff8374eb25)

---
### Motion Blending
https://graphics.cs.wisc.edu/Papers/2003/KG03/regCurves.pdf  
https://www.gamedeveloper.com/design/third-person-camera-view-in-games---a-record-of-the-most-common-problems-in-modern-games-solutions-taken-from-new-and-retro-games   
http://number-none.com/product/Understanding%20Slerp,%20Then%20Not%20Using%20It/   

#### Full Body Animation

#### Before Motion Blending
![Animation-Player-2023-07-13-22-50-38](https://github.com/Icefin/AnimationPlayer/assets/76864202/572a67e5-f720-4135-90ea-effe198f6ee3)

#### After Motion Blending
![Animation-Player-2023-07-13-22-34-50-_online-video-cutter com_](https://github.com/Icefin/AnimationPlayer/assets/76864202/3b1e8f33-2762-486e-91e3-9bcc932f6603)

A transition involves two motions and a weight function that
starts at (1,0) and smoothly changes to (0,1), and an interpolation combines an arbitrary number of motions according
to a constant weight function.

#### Animation Layering
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

![Animation-Player-2023-07-31-21-29-59](https://github.com/Icefin/AnimationPlayer/assets/76864202/a4c5eb7c-b083-471f-a654-25bf4e100500)

---
### Collision Detection System & Cloth Simulation
#### Reference :
https://graphics.stanford.edu/~mdfisher/cloth.html
https://learnopengl.com/Lighting/Basic-Lighting  
https://carmencincotti.com/2022-07-11/position-based-dynamics/
#### Requirements :
1. Simple Cloth Simulation with Hard-Coded Cube && Sphere
- Cloth simulation using Spring-MassPoint Model
2. Choose Lighting Model
- Phong Lighting Model : Not Enough for Multiple Objects || Single Concave Object
- Global Illmuination : Not Appropriate for Real-Time Rendering
- Phong Lighting Model + (Shadowing, Ambient Occlusion) -> Acceptable
3. Simple Cloth Simulation with Collider Cube && Sphere  
- Collision Detection && Resolution System is required
- Optimization is required from now......
  
4. Simple Cloth Simulation with Character
- Position Based Dynamics comes here
- How to simplify character with collider? -> Simplify Bones with OBB or Cylinder

5. Complex Cloth Simulation with Character
- Import .obj asset

#### GameObject && Geometric Primitives
```c++
struct OBB
{
	OBB(void) : position(0.0f, 0.0f, 0.0f), size(1.0f, 1.0f, 1.0f), orientation(1.0f, 0.0f, 0.0f, 0.0f) { }
	OBB(const glm::vec3& p, const glm::vec3& s) : position(p), size(s), orientation(1.0f, 0.0f, 0.0f, 0.0f) { }
	OBB(const glm::vec3& p, const glm::vec3& s, const glm::quat& q) : position(p), size(s), orientation(glm::normalize(q)) { }

	glm::vec3 position;
	glm::vec3 size;
	glm::quat orientation;
}

bool isOBBTriangleCollision(const OBB& obb, const Triangle& triangle);
bool isOBBPlaneCollision(const OBB& obb, const Plane& plane);
bool isOBBSphereCollision(const OBB& obb, const Sphere& sphere);
bool isOBBAABBCollision(const OBB& obb, const AABB& aabb);
bool isOBBOBBCollision(const OBB& o1, const OBB& o2);
```

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

struct DistanctConstraint
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
	void solveDistantConstraint(DistanctConstraint& constraint);
	void solveCollisionConstraint(CollisionConstraint& constraint);
	void updateMassPointNormal(void);

private:
	GLuint	_vao;
	GLuint	_vbo;
	GLuint	_ebo;

	std::vector<MassPoint>		_massPointList;
	std::vector<uint32>		_indices;
	std::vector<DistanctConstraint> _internalConstraints;

private:
	glm::vec3	_materialAmbient{0.1f, 0.1f, 0.1f};
	glm::vec3	_materialSpecular{0.2f, 0.2f, 0.2f};
	glm::vec3	_materialEmissive{0.0f, 0.0f, 0.0f};
	float		_materialShininess = 5.0f;
};
```

#### Collision Detection && Resolution
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
		for (DistanctConstraint& constraint : _internalConstraints)
			solveDistantConstraint(constraint);

		for (CollisionConstraint& constraint : collisionConstraints)
			solveCollisionConstraint(constraint);
	}

	//Velocity Update
	for (MassPoint& massPoint : _massPointList)
		massPoint.velocity = (massPoint.position - massPoint.prevPosition) / deltaTime;

	updateMassPointNormal();
}
```

```c++
void PlaneCloth::generateCollisionConstraint(MassPoint& massPoint, std::vector<pa::OBB> colliders, std::vector<CollisionConstraint>* collisionConstraints)
{
	for (pa::OBB& obb : colliders)
	{
		pa::Line travelPath(massPoint.prevPosition, massPoint.position);

		if (pa::isIntersection(travelPath, obb) == true)
		{
			massPoint.color = glm::vec3(1.0f, 0.0f, 0.0f);
			pa::Ray ray(massPoint.prevPosition, massPoint.position - massPoint.prevPosition);
			pa::RaycastInfo raycastInfo;
			pa::raycast(ray, obb, &raycastInfo);

			glm::vec3 targetPosition = raycastInfo.hitPoint + raycastInfo.normal * 0.05f;

			collisionConstraints->push_back({ targetPosition, &massPoint });
			return;
		}
		else if (pa::isPointInside(massPoint.position, obb) == true)
		{
			massPoint.color = glm::vec3(1.0f, 0.0f, 0.0f);
			pa::Ray ray(massPoint.position, massPoint.prevPosition - massPoint.position);
			pa::RaycastInfo raycastInfo;
			pa::raycast(ray, obb, &raycastInfo);

			glm::vec3 targetPosition = raycastInfo.hitPoint + raycastInfo.normal * 0.05f;
			
			collisionConstraints->push_back({ targetPosition, &massPoint });
			return;
		}
	}
}
```

```c++
void PlaneCloth::solveDistantConstraint(DistanctConstraint& constraint)
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

```c++
bool isIntersection(const Line& line, const OBB& obb)
{
	Ray ray;
	ray.origin = line.p1;
	ray.direction = glm::normalize(line.p2 - line.p1);

	RaycastInfo raycastInfo;
	raycast(ray, obb, &raycastInfo);

	float squareLength = calculateSquareLength(line);

	return (raycastInfo.isHit && raycastInfo.rayTime * raycastInfo.rayTime < squareLength);
}

void raycast(const Ray& ray, const OBB& obb, RaycastInfo* outInfo)
{
	resetRaycastInfo(outInfo);

	glm::vec3 halfSide = obb.size;
	glm::mat3 rotation = glm::mat3(obb.orientation);
	glm::vec3 basis[3] = {
		glm::vec3(rotation[0][0], rotation[0][1], rotation[0][2]),
		glm::vec3(rotation[1][0], rotation[1][1], rotation[1][2]),
		glm::vec3(rotation[2][0], rotation[2][1], rotation[2][2])
	};

	glm::vec3 rayToOBB = obb.position - ray.origin;
	glm::vec3 f = glm::vec3(glm::dot(basis[0], ray.direction), glm::dot(basis[1], ray.direction), glm::dot(basis[2], ray.direction));
	glm::vec3 e = glm::vec3(glm::dot(basis[0], rayToOBB), glm::dot(basis[1], rayToOBB), glm::dot(basis[2], rayToOBB));

	float t[6] = { 0, 0, 0, 0, 0, 0 };
	for (int32 i = 0; i < 3; ++i)
	{
		if (glm::abs(f[i]) < EPSILON)
		{
			if (-e[i] - halfSide[i] > 0.0f || -e[i] + halfSide[i] < 0.0f)
				return;

			f[i] = 0.00001f;
		}

		t[i * 2] = (e[i] + halfSide[i]) / f[i];
		t[i * 2 + 1] = (e[i] - halfSide[i]) / f[i];
	}

	float tmin = fmaxf(fmaxf(fminf(t[0], t[1]), fminf(t[2], t[3])), fminf(t[4], t[5]));
	float tmax = fminf(fminf(fmaxf(t[0], t[1]), fmaxf(t[2], t[3])), fmaxf(t[4], t[5]));

	if (tmax < 0.0f)
		return;

	if (tmin > tmax)
		return;

	float tresult = tmin;
	if (tmin < 0.0f)
		tresult = tmax;

	if (outInfo != nullptr)
	{
		outInfo->isHit = true;
		outInfo->rayTime = tresult;
		outInfo->hitPoint = ray.origin + tresult * ray.direction;

		glm::vec3 normals[6] = {
			basis[0], -basis[0], basis[1], -basis[1], basis[2], -basis[2]
		};

		for (int32 i = 0; i < 6; ++i)
		{
			if (glm::abs(tresult - t[i]) < EPSILON)
				outInfo->normal = glm::normalize(normals[i]);
		}
	}
}
```

#### Result :
![res](https://github.com/Icefin/AnimationPlayer/assets/76864202/cba4d17a-b822-4561-ae40-b9d96c681ab5)
![temp](https://github.com/Icefin/AnimationPlayer/assets/76864202/cfb009d6-85f5-4c1b-8ee1-77c192c3f1f8)
![image](https://github.com/Icefin/AnimationPlayer/assets/76864202/90ea8cc9-f978-487f-a1b6-02c09e042c1e)

First step with Fixed Volume + Explicit Euler Method

https://github.com/Icefin/AnimationPlayer/assets/76864202/c73974bf-4ded-408e-887c-cfa4673ea058

Second try with Collision Detection + Explicit Euler Method

https://github.com/Icefin/AnimationPlayer/assets/76864202/6b94f60f-0e0a-459d-a148-ab901196c361

Third try with Collision Detection + Verlet Integration

https://github.com/Icefin/AnimationPlayer/assets/76864202/2ed6625b-3beb-4a9a-83d4-e79fb19f375a

Forth try with PBD

https://github.com/Icefin/AnimationPlayer/assets/76864202/78efec86-4454-4287-b4c5-90723416f04e



#### Optimization
Constraint - Vertex Number Relation (Force based Simulation)
|     | 10 * 10 | 20 * 20 | 30 * 30 | 40 * 40 |
|:---:|:-------:|:-------:|:-------:|:-------:|
|**1**| 0.016ms | 0.016ms | 0.027ms | 0.047ms |
|**2**| 0.016ms | 0.016ms | 0.034ms | 0.057ms |
|**3**| 0.016ms | 0.018ms | 0.038ms | 0.067ms |
|**4**| 0.016ms | 0.020ms | 0.045ms | 0.077ms |
|**5**| 0.016ms | 0.023ms | 0.050ms | 0.087ms |

Iteration Count - Vertex Number Relation (Position based Simulation)
|     | 10 * 10 | 20 * 20 | 30 * 30 | 40 * 40 |
|:---:|:-------:|:-------:|:-------:|:-------:|
|**1**| 0.016ms | 0.028ms | 0.061ms | 0.110ms |
|**2**| 0.016ms | 0.030ms | 0.067ms | 0.120ms |
|**3**| 0.017ms | 0.033ms | 0.073ms | 0.130ms |
|**4**| 0.017ms | 0.035ms | 0.080ms | 0.143ms |
|**5**| 0.017ms | 0.039ms | 0.086ms | 0.155ms |

- BVH for character mesh
- Jacobi rather than Gauss-Seidel
- Parallelize Normal Calculation

#### Errors
![image](https://github.com/Icefin/AnimationPlayer/assets/76864202/278cdc7d-738e-4ddb-abf3-69a79eeb8c79)
![image](https://github.com/Icefin/AnimationPlayer/assets/76864202/a5b32d76-ee24-4b11-b26e-9b13327b5841)

- Shadowing
- Thickness or Spherical Collider to Particles to prevent folding (Spatial Hashing for cloth particle)


---
### Entity-Componenet-System Architecture
#### Requirements :
Blah Blah...  

---
### OBJ Importer && Skinning
#### Requirements :
Blah Blah...


---
### Particle System
#### Requirements :
1. Spatial Partitioning... Blah Blah...
