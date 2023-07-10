// author: Geonho Shin (icefin@pearlabyss.com)
#pragma once
#include <vector>
#include <unordered_map>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "CommonTypes.h"

enum class AXIS_TYPE
{
	XYZ, XZY, YXZ, YZX, ZXY, ZYX
};

struct ASFBone
{
	ASFBone*				parentBone;
	std::vector<ASFBone*>	childList;

	int32					boneIndex;
	glm::vec3				direction = glm::vec3(0.0f);
	glm::vec3				orientation = glm::vec3(0.0f);
	float					length = 0.0f;
	AXIS_TYPE				axisType;

	int32					dof = 0;
	bool					doftx = false, dofty = false, doftz = false;
	bool					dofrx = false, dofry = false, dofrz = false;
	bool					dofl = false;
	int32					dofo[8] = {0};

	glm::mat4				toParent = glm::mat4(1.0f);
};

struct ASFData
{
	float					mass = 1.0f;
	float					length = 1.0f;
	bool					isRadian = true;

	int32					totalBoneNumber = 0;
	int32					movingBoneNumber = 0;

	std::unordered_map<std::string, ASFBone> boneMap;
};

class ASFParser
{
public:
	~ASFParser();
	ASFData* readASF(std::string& filename);

private:
	void	readASF_Units(std::ifstream& stream, std::string& buffer);
	void	readASF_Root(std::ifstream& stream, std::string& buffer);
	void	readASF_Bonedata(std::ifstream& stream, std::string& buffer);
	void	readASF_Hierarchy(std::ifstream& stream);
	//void	readASF_Skin(std::ifstream& stream, std::string& buffer);

	ASFData	 _asfData;
};

//주석 예시.(20230706:신건호)