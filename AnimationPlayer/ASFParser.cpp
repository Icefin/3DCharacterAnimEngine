// author: Geonho Shin (icefin@pearlabyss.com)
#include <fstream>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ASFParser.h"
#include "CustomMath.h"

ASFParser::~ASFParser()
{
	__noop;
}

ASFData* ASFParser::readASF(std::string& filename)
{
	ASFData* asfData = new ASFData;
	std::ifstream stream;
	stream.open(filename, std::ios_base::in);
	if (stream.fail() == true)
		return NULL;

	std::string buffer;

	while (stream.eof() == false)
	{
		stream >> buffer;

		if (buffer == ":units")
			readASF_Units(stream, buffer, asfData);
		if (buffer == ":root")
			readASF_Root(stream, buffer, asfData);
		if (buffer == ":bonedata")
			readASF_Bonedata(stream, buffer, asfData);
		if (buffer == ":hierarchy")
			readASF_Hierarchy(stream, asfData);
		//else if (buffer.find(":skin") != std::string::npos)
		std::getline(stream, buffer);
	}

	stream.close();

	return asfData;
}

void ASFParser::readASF_Units(std::ifstream& stream, std::string& buffer, ASFData* asfData)
{
	while (true)
	{
		stream >> buffer;
		if (buffer == ":root")
			return;
		
		if (buffer == "mass")
			stream >> asfData->mass;
		else if (buffer == "length")
			stream >> asfData->length;
		else if (buffer == "angle")
		{
			stream >> buffer;
			if (buffer == "deg")
				asfData->isRadian = false;
		}

		std::getline(stream, buffer);
	}
}

void ASFParser::readASF_Root(std::ifstream& stream, std::string& buffer, ASFData* asfData)
{
	ASFBone root;
	while (buffer != ":bonedata")
	{
		if (buffer != "axis")
			stream >> buffer;
		if (buffer[0] == '#')
		{
			std::getline(stream, buffer);
			continue;
		}

		if (buffer == "axis")
		{
			stream >> buffer;
			if (buffer == "XYZ")
				root.axisType = AXIS_TYPE::XYZ;
			else if (buffer == "XZY")
				root.axisType = AXIS_TYPE::XZY;
			else if (buffer == "YXZ")
				root.axisType = AXIS_TYPE::YXZ;
			else if (buffer == "YZX")
				root.axisType = AXIS_TYPE::YZX;
			else if (buffer == "ZXY")
				root.axisType = AXIS_TYPE::ZXY;
			else if (buffer == "ZYX")
				root.axisType = AXIS_TYPE::ZYX;
		}
		else if (buffer == "order")
		{
			int32 idof = 0;
			while (buffer != "axis")
			{
				stream >> buffer;
				if (buffer == "RX") { root.dofrx = true; root.dofo[idof++] = 1; }
				else if (buffer == "RY") { root.dofry = true; root.dofo[idof++] = 2; }
				else if (buffer == "RZ") { root.dofrz = true; root.dofo[idof++] = 3; }
				else if (buffer == "TX") { root.doftx = true; root.dofo[idof++] = 4; }
				else if (buffer == "TY") { root.dofty = true; root.dofo[idof++] = 5; }
				else if (buffer == "TZ") { root.doftz = true; root.dofo[idof++] = 6; }
				else if (buffer == "L") { root.dofl = true; root.dofo[idof++] = 7; }
			}
			root.dof = idof;
			if (idof > 0)
				asfData->movingBoneNumber++;
		}
		else if (buffer == "position")
		{
			stream >> root.direction.x;
			stream >> root.direction.y;
			stream >> root.direction.z;
		}
		else if (buffer == "orientation")
		{
			stream >> root.orientation.x;
			stream >> root.orientation.y;
			stream >> root.orientation.z;
			if (asfData->isRadian == false)
			{
				root.orientation.x = glm::radians(root.orientation.x);
				root.orientation.y = glm::radians(root.orientation.y);
				root.orientation.z = glm::radians(root.orientation.z);
			}
		}
	}
	root.boneIndex = 0;
	asfData->boneMap["root"] = root;
	asfData->totalBoneNumber++;
}

void ASFParser::readASF_Bonedata(std::ifstream& stream, std::string& buffer, ASFData* asfData)
{
	while (buffer != ":hierarchy")
	{
		stream >> buffer;

		if (buffer == "begin")
		{
			ASFBone bone;
			std::string name;
			while (buffer != "end")
			{
				stream >> buffer;
				if (buffer[0] == '#')
				{
					std::getline(stream, buffer);
					continue;
				}

				if (buffer == "name")
					stream >> name;
				else if (buffer == "direction")
				{
					stream >> bone.direction.x;
					stream >> bone.direction.y;
					stream >> bone.direction.z;
				}
				else if (buffer == "length")
					stream >> bone.length;
				else if (buffer == "axis")
				{
					stream >> bone.orientation.x;
					stream >> bone.orientation.y;
					stream >> bone.orientation.z;
					if (asfData->isRadian == false)
					{
						bone.orientation.x = glm::radians(bone.orientation.x);
						bone.orientation.y = glm::radians(bone.orientation.y);
						bone.orientation.z = glm::radians(bone.orientation.z);
					}
					stream >> buffer;
					if (buffer == "XYZ")
						bone.axisType = AXIS_TYPE::XYZ;
					else if (buffer == "XZY")
						bone.axisType = AXIS_TYPE::XZY;
					else if (buffer == "YXZ")
						bone.axisType = AXIS_TYPE::YXZ;
					else if (buffer == "YZX")
						bone.axisType = AXIS_TYPE::YZX;
					else if (buffer == "ZXY")
						bone.axisType = AXIS_TYPE::ZXY;
					else if (buffer == "ZYX")
						bone.axisType = AXIS_TYPE::ZYX;
				}
				else if (buffer == "dof")
				{
					int32 idof = 0;
					while (buffer != "limits")
					{
						stream >> buffer;
						if (buffer == "rx") { bone.dofrx = true; bone.dofo[idof++] = 1; }
						else if (buffer == "ry") { bone.dofry = true; bone.dofo[idof++] = 2; }
						else if (buffer == "rz") { bone.dofrz = true; bone.dofo[idof++] = 3; }
						else if (buffer == "tx") { bone.doftx = true; bone.dofo[idof++] = 4; }
						else if (buffer == "ty") { bone.dofty = true; bone.dofo[idof++] = 5; }
						else if (buffer == "tz") { bone.doftz = true; bone.dofo[idof++] = 6; }
						else if (buffer == "l") { bone.dofl = true; bone.dofo[idof++] = 7; }
					}
					bone.dof = idof;
					if (idof > 0)
						asfData->movingBoneNumber++;
				}
				else if (buffer == "limits")
				{
					__noop;
				}
				else if (buffer == "bodymass")
				{
					__noop;
				}
				else if (buffer == "cofmass")
				{
					__noop;
				}
			}
			bone.boneIndex = asfData->totalBoneNumber;
			asfData->boneMap[name] = bone;
			asfData->totalBoneNumber++;
		}
	}
}

void ASFParser::readASF_Hierarchy(std::ifstream& stream, ASFData* asfData)
{
	std::string parentName;
	std::string childName;

	stream >> parentName;
	while (parentName != "end")
	{
		if (parentName[0] != '#' && parentName != "begin")
		{
			std::getline(stream, childName);

			std::string name;

			int32 pos = childName.find(" ");
			childName.erase(0, pos + 1);

			pos = childName.find(" ");
			while (pos != std::string::npos)
			{
				std::string name = childName.substr(0, pos);
				asfData->boneMap[parentName].childList.push_back(&(asfData->boneMap[name]));
				asfData->boneMap[name].parentIndex = asfData->boneMap[parentName].boneIndex;
				childName.erase(0, pos + 1);
				pos = childName.find(" ");
			}
			asfData->boneMap[parentName].childList.push_back(&(asfData->boneMap[childName]));
			asfData->boneMap[childName].parentIndex = asfData->boneMap[parentName].boneIndex;
		}
		
		if (parentName[0] == '#')
			std::getline(stream, parentName);
		stream >> parentName;
	}
	asfData->boneMap["root"].parentIndex = -1;
}