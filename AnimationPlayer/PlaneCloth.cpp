#include <algorithm>

#include "PlaneCloth.h"

PlaneCloth::PlaneCloth(glm::vec3 position, glm::vec3 color, uint32 width, uint32 height, uint32 widthNum, uint32 heightNum)
{
	_position = position;

	float dw = static_cast<float>(width) / static_cast<float>(widthNum);
	float dh = static_cast<float>(height) / static_cast<float>(heightNum);
	
	_massPointList.resize(widthNum * heightNum);
	// MassPoint Initialize
	for (int32 h = 0; h < heightNum; ++h)
	{
		for (int32 w = 0; w < widthNum; ++w)
		{
			MassPoint newPoint;
			newPoint.invMass = 1.0f;
			newPoint.prevPosition = glm::vec3(position.x + w * dw, position.y, position.z + h * dh);
			newPoint.position = glm::vec3(position.x + w * dw, position.y, position.z + h * dh);
			newPoint.velocity = glm::vec3(0.0f, 0.0f, 0.0f);
			newPoint.color = color;

			_massPointList[w + h * widthNum] = newPoint;
		}
	}
	_massPointList[0].invMass = 0.0f;
	_massPointList[widthNum - 1].invMass = 0.0f;

	// StructuralSpring Initialize
	for (int32 h = 0; h < heightNum - 1; ++h)
	{
		for (int32 w = 0; w < widthNum - 1; ++w)
		{
			MassPoint* origin = &_massPointList[w + h * widthNum];
			MassPoint* right = &_massPointList[(w + 1) + h * widthNum];
			MassPoint* lower = &_massPointList[w + (h + 1) * widthNum];
			
			float restLengthRightward = glm::distance(origin->position, right->position);
			float restLengthDownward = glm::distance(origin->position, lower->position);

			_internalConstraints.push_back({ restLengthRightward, origin, right });
			_internalConstraints.push_back({ restLengthDownward, origin, lower });
		}
	}

	for (int32 w = 0; w < widthNum - 1; ++w)
	{
		MassPoint* origin = &_massPointList[w + (heightNum - 1) * widthNum];
		MassPoint* right = &_massPointList[(w + 1) + (heightNum - 1) * widthNum];

		float restLengthRightward = glm::distance(origin->position, right->position);

		_internalConstraints.push_back({ restLengthRightward, origin, right });
	}

	for (int32 h = 0; h < heightNum - 1; ++h)
	{
		MassPoint* origin = &_massPointList[widthNum - 1 + h * widthNum];
		MassPoint* lower = &_massPointList[widthNum - 1 + (h + 1) * widthNum];

		float restLengthDownward = glm::distance(origin->position, lower->position);

		_internalConstraints.push_back({ restLengthDownward, origin, lower });
	}

	// ShearSpring Initialize
	for (int32 h = 0; h < heightNum - 1; ++h)
	{
		for (int32 w = 0; w < widthNum - 1; ++w)
		{
			MassPoint* origin = &_massPointList[w + h * widthNum];
			MassPoint* lowerRight = &_massPointList[(w + 1) + (h + 1) * widthNum];

			float restLengthLowerRight = glm::distance(origin->position, lowerRight->position);

			_internalConstraints.push_back({ restLengthLowerRight, origin, lowerRight });
		}
	}

	for (int32 h = 0; h < heightNum - 1; ++h)
	{
		for (int32 w = 1; w < widthNum; ++w)
		{
			MassPoint* origin = &_massPointList[w + h * widthNum];
			MassPoint* lowerLeft = &_massPointList[(w - 1) + (h + 1) * widthNum];

			float restLengthLowerLeft = glm::distance(origin->position, lowerLeft->position);

			_internalConstraints.push_back({ restLengthLowerLeft, origin, lowerLeft });
		}
	}

	// BendSpring Initialize
	for (int32 h = 0; h < heightNum; ++h)
	{
		for (int32 w = 0; w < widthNum - 2; ++w)
		{
			MassPoint* origin = &_massPointList[w + h * widthNum];
			MassPoint* distRight = &_massPointList[(w + 2) + h * widthNum];

			float restLengthDistRight = glm::distance(origin->position, distRight->position);

			_internalConstraints.push_back({ restLengthDistRight, origin, distRight });
		}
	}

	for (int32 h = 0; h < heightNum - 2; ++h)
	{
		for (int32 w = 0; w < widthNum; ++w)
		{
			MassPoint* origin = &_massPointList[w + h * widthNum];
			MassPoint* distLower = &_massPointList[w + (h + 2) * widthNum];

			float restLengthDistRight = glm::distance(origin->position, distLower->position);

			_internalConstraints.push_back({ restLengthDistRight, origin, distLower });
		}
	}

	// Indices Initialize
	for (uint32 h = 0; h < heightNum - 1; ++h)
	{
		for (uint32 w = 0; w < widthNum - 1; ++w)
		{
			_indices.push_back(w + h * widthNum);
			_indices.push_back((w + 1) + (h + 1) * widthNum);
			_indices.push_back((w + 1) + h * widthNum);

			_indices.push_back(w + h * widthNum);
			_indices.push_back(w + (h + 1) * widthNum);
			_indices.push_back((w + 1) + (h + 1) * widthNum);
		}
	}

	// GLContext Initialize
	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	glGenBuffers(1, &_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, _massPointList.size() * sizeof(MassPoint), _massPointList.data(), GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(MassPoint), (void*)offsetof(MassPoint, position));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(MassPoint), (void*)offsetof(MassPoint, color));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(MassPoint), (void*)offsetof(MassPoint, normal));

	glGenBuffers(1, &_ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(uint32), _indices.data(), GL_STATIC_DRAW);
}

PlaneCloth::~PlaneCloth(void)
{
	glDeleteVertexArrays(1, &_vao);
	glDeleteBuffers(1, &_vbo);
	glDeleteBuffers(1, &_ebo);
}

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

void PlaneCloth::render(Shader& shader)
{
	glm::mat4 worldMat = glm::mat4(1.0f);
	shader.setUniformVec3("material.ambient", _materialAmbient);
	shader.setUniformVec3("material.emissive", _materialEmissive);
	shader.setUniformVec3("material.specular", _materialSpecular);
	shader.setUniformFloat("material.shininess", _materialShininess);
	shader.setUniformMat4("worldMat", worldMat);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glBindVertexArray(_vao);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, _massPointList.size() * sizeof(MassPoint), _massPointList.data());
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
	glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, NULL);
}

void PlaneCloth::updateMassPointNormal(void)
{
	int32 numMassPoints = _massPointList.size();
	std::vector<std::vector<glm::vec3>> normals(numMassPoints, std::vector<glm::vec3>(6));
	int32 numIndices = _indices.size();
	for (int32 i = 0; i < numIndices; i += 6) {

		glm::vec3 v1 = _massPointList[_indices[i + 1]].position - _massPointList[_indices[i]].position;
		glm::vec3 v2 = _massPointList[_indices[i + 2]].position - _massPointList[_indices[i]].position;

		glm::vec3 v3 = _massPointList[_indices[i + 4]].position - _massPointList[_indices[i + 3]].position;
		glm::vec3 v4 = _massPointList[_indices[i + 5]].position - _massPointList[_indices[i + 3]].position;

		glm::vec3 normal1 = glm::cross(v1, v2);
		normal1 = glm::normalize(normal1);

		glm::vec3 normal2 = glm::cross(v3, v4);
		normal2 = glm::normalize(normal2);

		normals[_indices[i]].push_back(normal1);
		normals[_indices[i + 1]].push_back(normal1);
		normals[_indices[i + 2]].push_back(normal1);
		normals[_indices[i + 3]].push_back(normal2);
		normals[_indices[i + 4]].push_back(normal2);
		normals[_indices[i + 5]].push_back(normal2);
	}

	for (int32 i = 0; i < numMassPoints; ++i) {
		glm::vec3 avg{ 0.0f, 0.0f, 0.0f };

		for (int32 j = 0; j < normals[i].size(); ++j)
			avg += normals[i][j];

		avg = glm::normalize(avg);
		_massPointList[i].normal = avg;
	}
}
