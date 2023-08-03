# AnimationPlayer
 Simple Character Animator

---
### Project Architecture


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

#### Partial Body Animation
![Animation-Player-2023-07-31-21-29-59](https://github.com/Icefin/AnimationPlayer/assets/76864202/a4c5eb7c-b083-471f-a654-25bf4e100500)

---
### Collision Detection System & Cloth Simulation
#### Reference :

#### Requirements :
1. Simple Cloth Simulation with Hard-Coded Cube && Sphere
- Cloth simulation using Spring-MassPoint Model
2. Choose Lighting Model
- Phong Lighting Model : Not Enough for Cloth
- Global Illmuination : Not Appropriate for Real-Time Rendering
- Phong Lighting Model + Ambient Occlusion -> Acceptable
3. Simple Cloth Simulation with Collider Cube && Sphere
- Collision Detection System....!!
4. Simple Cloth Simulation with Character
5. Complex Cloth Simulation with Character

#### GameObject && Geometric Primitives
```c++
class GameObject
{
public :
	virtual void update(float deltaTime) = 0;
	virtual void render(Shader& shader) = 0;

	uint32		_objectID;
	glm::vec3	_position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::quat	_rotation = glm::vec3(0.0f, 0.0f, 0.0f);
};
```

```c++
class Cube : public GameObject
{
public :
        Cube(glm::vec3 position, glm::vec3 halfSideLength, glm::vec3 color = glm::vec3(0.9f, 0.9f, 0.9f));
        ~Cube(void);
	
	void	update(float deltaTime) override;
	void	render(Shader& shader) override;

private :
	GLuint	_vbo;
	GLuint	_vao;
	GLuint	_ebo;
};

class Sphere : public GameObject
{
public :
	Sphere(glm::vec3 position, float radius, glm::vec3 color = glm::vec3(0.9f, 0.9f, 0.9f), uint32 stackNumber = 20, uint32 sectorNumber = 20);
	~Sphere(void);

	void	update(float deltaTime) override;
	void	render(Shader& shader) override;

private:
	GLuint	_vbo;
	GLuint	_vao;
	GLuint	_ebo;

	int32	_indicesNumber;
};
```

```c++
struct MassPoint
{
	float		mass;
	glm::vec3	position;
	glm::vec3	velocity;
	glm::vec3	netForce;
	glm::vec3	color;
};

enum class SpringType :uint8
{
	Structural = 0,
	Shear,
	Flexion
};

struct Spring
{
	SpringType type;
	float restLength;
	MassPoint* left;
	MassPoint* right;
};

class PlaneCloth : public GameObject
{
public :
	PlaneCloth(glm::vec3 position, uint32 width, uint32 height, uint32 widthNum, uint32 heightNum);
	~PlaneCloth(void);

	void update(float deltaTime) override;
	void render(Shader& shader) override;

private :
	void applyInternalForces(void);
	void applyExternalForces(void);
	void updateMassPointState(float deltaTime);
	void solveCollision(void);

private :
	GLuint	_vao;
	GLuint	_vbo;
	GLuint	_ebo;

	std::vector<MassPoint>	_massPointList;
	std::vector<uint32>	_indices;
	std::vector<Spring>	_springList;
};
```

#### Lighting System
Phong Lighting Model + (Ambient Occlusion)

#### Collision Detection && Solving

#### Game-Loop

#### Result(Temp) :

---
### Particle System
#### Requirements :
1. Spatial Partitioning... Blah Blah...
