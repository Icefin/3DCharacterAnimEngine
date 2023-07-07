// author: Geonho Shin (icefin@pearlabyss.com)
#include <fstream>
#include <string>
#include "AMCParser.h"

AMCParser::~AMCParser()
{
	__noop;
}

AMCData* AMCParser::readAMC(std::string& filename, ASFData* asfData)
{
	std::ifstream stream(filename, std::ios::in);
	if (stream.fail() == true)
		return NULL;

	std::string buffer;
	stream >> buffer;
	while (buffer != "1")
	{
		std::getline(stream, buffer);
		stream >> buffer;
	}

	_amcData.boneMotions = std::vector<std::vector<AMCPosture>>(asfData->totalBoneNumber);
	int32 totalBoneNumber = asfData->totalBoneNumber;
	int32 movingBoneNumber = asfData->movingBoneNumber;

	while (stream.eof() == false)
	{
		for (int32 i = 0; i < movingBoneNumber; ++i)
		{
			std::string name;
			stream >> name;

			AMCPosture newPosture;
			ASFBone& bone = asfData->boneMap[name];
			int32	boneDof = bone.dof;
			int32	boneIndex = bone.boneIndex;
			for (int32 j = 0; j < boneDof; ++j)
			{
				float temp;
				stream >> temp;

				switch (bone.dofo[j])
				{
				case 1:
					{
						if (asfData->isRadian == false)
							newPosture.frameRotation.x = glm::radians(temp);
						else
							newPosture.frameRotation.x = temp;
						
					}
					break;
				case 2:
					{
						if (asfData->isRadian == false)
							newPosture.frameRotation.y = glm::radians(temp);
						else
							newPosture.frameRotation.y = temp;
					}
					break;
				case 3:
					{
						if (asfData->isRadian == false)
							newPosture.frameRotation.z = glm::radians(temp);
						else
							newPosture.frameRotation.z = temp;
					}
					break;
				case 4:
					{
						newPosture.frameTranslation.x = temp;
					}
					break;
				case 5:
					{
						newPosture.frameTranslation.y = temp;
					}
					break;
				case 6:
					{
						newPosture.frameTranslation.z = temp;
					}
					break;
				case 7:
					{
						newPosture.frameLocation = temp;
					}
					break;
				}
			}
			_amcData.boneMotions[boneIndex].push_back(newPosture);
		}
		stream >> buffer;
		std::getline(stream, buffer);
		_amcData.totalFrameNumber++;
	}

	stream.close();

	return (&_amcData);
}