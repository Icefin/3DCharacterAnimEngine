#pragma once
#include <vector>
#include <unordered_map>
#include "CommonTypes.h"
#include "ASFParser.h"

struct AMCPosture
{
	vec3f	frameRotation;
	vec3f	frameTranslation;
	float	frameLocation = 0.0f;
};

struct AMCData
{
	int32					totalFrameNumber = 0;

	std::unordered_map<std::string, std::vector<AMCPosture>> boneMotions;
};

class AMCParser
{
public:
	AMCData* readAMC(std::string& filename, ASFData* asfData);

private:
	AMCData _amcData;
};

