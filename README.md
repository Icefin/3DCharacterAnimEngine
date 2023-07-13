# AnimationPlayer
 Simple Character Animator

### Parsing ASF / AMC Data
#### Reference :
https://research.cs.wisc.edu/graphics/Courses/cs-838-1999/Jeff/ASF-AMC.html  
http://www.cs.cmu.edu/~kiranb/animation/StartupCodeDescription.htm  
http://graphics.cs.cmu.edu/nsp/course/cs229/info/Acclaim_Skeleton_Format.html  


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
```
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
```
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
```
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
1. Threshold = 0.01f / Average 1086 -> 457
![Animation-Player-2023-07-12-23-04-36](https://github.com/Icefin/AnimationPlayer/assets/76864202/4c3d9d69-38cb-4ba1-94f5-6236793b4b67)

2. Threshold = 0.1f  / Average 1086 -> 105
![Animation-Player-2023-07-12-23-12-56](https://github.com/Icefin/AnimationPlayer/assets/76864202/716ee92f-183e-47c4-9aeb-eeff8374eb25)




### Motion Blending
https://graphics.cs.wisc.edu/Papers/2003/KG03/regCurves.pdf  
https://www.gamedeveloper.com/design/third-person-camera-view-in-games---a-record-of-the-most-common-problems-in-modern-games-solutions-taken-from-new-and-retro-games  
https://github.com/LaivY  
http://number-none.com/product/Understanding%20Slerp,%20Then%20Not%20Using%20It/  




A transition involves two motions and a weight function that
starts at (1,0) and smoothly changes to (0,1), and an interpolation combines an arbitrary number of motions according
to a constant weight function.

Linear blending produces reasonable results when the input motions are sufficiently similar. Our strategy is thus
to still combine frames via averaging, but to automatically
extract information from the input motions to help decide
which frames to combine, how to position and orient them
prior to averaging, and what constraints should exist on the
result. This information is used to create a timewarp curve, a
coordinate alignment curve, and a set of constraint matches,
which together form a registration curve.
