#pragma once
#include <vector>
#include <unordered_map>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "CommonTypes.h"
#include "ASFParser.h"

struct AMCPosture
{
	glm::vec3	frameRotation;
	glm::vec3	frameTranslation;
	float		frameLocation = 0.0f;
};

struct AMCData
{
	int32					totalFrameNumber = 0;

	std::unordered_map<std::string, std::vector<AMCPosture>> boneMotions;
};

class AMCParser
{
public:
	~AMCParser();
	AMCData* readAMC(std::string& filename, ASFData* asfData);

private:
	AMCData _amcData;
};

