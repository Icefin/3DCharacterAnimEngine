#include <fstream>
#include <string>
#include "AMCParser.h"

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

	int32 totalBoneNumber = asfData->totalBoneNumber;
	int32 movingBoneNumber = asfData->movingBoneNumber;

	while (stream.eof() == false)
	{
		for (int32 i = 0; i < movingBoneNumber; i++)
		{
			std::string name;
			stream >> name;

			AMCPosture newPosture;
			ASFBone& bone = asfData->boneMap[name];
			int32	boneDof = bone.dof;
			for (int32 j = 0; j < boneDof; j++)
			{
				float temp;
				stream >> temp;

				switch (bone.dofo[j])
				{
				case 1:
					newPosture.frameRotation.x = temp;
					break;
				case 2:
					newPosture.frameRotation.y = temp;
					break;
				case 3:
					newPosture.frameRotation.z = temp;
					break;
				case 4:
					newPosture.frameTranslation.x = temp;
					break;
				case 5:
					newPosture.frameTranslation.y = temp;
					break;
				case 6:
					newPosture.frameTranslation.z = temp;
					break;
				case 7:
					newPosture.frameLocation = temp;
					break;
				}
			}
			_amcData.boneMotions[name].push_back(newPosture);
		}
		stream >> buffer;
		std::getline(stream, buffer);
		_amcData.totalFrameNumber++;
	}

	stream.close();

	return (&_amcData);
}