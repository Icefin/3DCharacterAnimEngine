# AnimationPlayer
 Simple Character Animator

Catmull-rom Spline

Barry-Goldman Algorithm...?
Reference :
https://www.cs.cmu.edu/~fp/courses/graphics/asst5/catmullRom.pdf
https://splines.readthedocs.io/en/latest/euclidean/catmull-rom-barry-goldman.html

Requirements:
1. Interpolate each Quaternion elements vs {Interpolate whole together} ?
	Interpolating whole together is much easy and compact.

2. Which data structure to use ?
vector<pair<frame, glm::quat>> ControlPoints (circular vector lol) -> Useless....
vector<pair<frame, glm::quat>> keyFrames (List (insertion!) vs Vector (BS!))
				List : Search : O(n), Insertion : O(1)
				Vector : Search : O(logn), Insertion : O(n)

struct CompressedAnimation
{
	uint16 keyFrame;
	QuantizedQuaternion rotation;
}

3. Do we need a new Compressor Class ?
	I think not.... yet...?

Final format :
Motion.h
public :
Posture getBonePostureAtFrame(int32 boneIndex, float time);
void	setBoneCompressedAnimation(int32 boneIndex, CompressedAnimation anim);

private :
Posture decompressAnimationData(CompressedAnimation anim);
std::vector<std::vector<CompressedAnimation>> _keyFrameAnimations;


CharacterLoader.h
Motion* generateMotion(CompressedData* data, int32 totalBoneNumber);