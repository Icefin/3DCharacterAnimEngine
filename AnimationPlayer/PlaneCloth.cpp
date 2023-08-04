#include <algorithm>

#include "PlaneCloth.h"

PlaneCloth::PlaneCloth(glm::vec3 position, uint32 width, uint32 height, uint32 widthNum, uint32 heightNum, bool test)
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
			newPoint.mass = 1.0f;
			newPoint.position = glm::vec3(w * dw, 0, h * dh);
			newPoint.velocity = glm::vec3(0.0f, 0.0f, 0.0f);
			newPoint.netForce = glm::vec3(0.0f, 0.0f, 0.0f);
			newPoint.color = glm::vec3(0.9f, 0.9f, 0.9f);

			_massPointList[w + h * widthNum] = newPoint;
		}
	}

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

			_springList.push_back({ SpringType::Structural, restLengthRightward, origin, right });
			_springList.push_back({ SpringType::Structural, restLengthDownward, origin, lower });
		}
	}

	for (int32 w = 0; w < widthNum - 1; ++w)
	{
		MassPoint* origin = &_massPointList[w + (heightNum - 1) * widthNum];
		MassPoint* right = &_massPointList[(w + 1) + (heightNum - 1) * widthNum];

		float restLengthRightward = glm::distance(origin->position, right->position);

		_springList.push_back({ SpringType::Structural, restLengthRightward, origin, right });
	}

	for (int32 h = 0; h < heightNum - 1; ++h)
	{
		MassPoint* origin = &_massPointList[widthNum - 1 + h * widthNum];
		MassPoint* lower = &_massPointList[widthNum - 1 + (h + 1) * widthNum];

		float restLengthDownward = glm::distance(origin->position, lower->position);

		_springList.push_back({ SpringType::Structural, restLengthDownward, origin, lower });
	}

	// ShearSpring Initialize
	for (int32 h = 0; h < heightNum - 1; ++h)
	{
		for (int32 w = 0; w < widthNum - 1; ++w)
		{
			MassPoint* origin = &_massPointList[w + h * widthNum];
			MassPoint* lowerRight = &_massPointList[(w + 1) + (h + 1) * widthNum];

			float restLengthLowerRight = glm::distance(origin->position, lowerRight->position);

			_springList.push_back({ SpringType::Shear, restLengthLowerRight, origin, lowerRight });
		}
	}

	for (int32 h = 0; h < heightNum - 1; ++h)
	{
		for (int32 w = 1; w < widthNum; ++w)
		{
			MassPoint* origin = &_massPointList[w + h * widthNum];
			MassPoint* lowerLeft = &_massPointList[(w - 1) + (h + 1) * widthNum];

			float restLengthLowerLeft = glm::distance(origin->position, lowerLeft->position);

			_springList.push_back({ SpringType::Shear, restLengthLowerLeft, origin, lowerLeft });
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

			_springList.push_back({ SpringType::Flexion, restLengthDistRight, origin, distRight });
		}
	}

	for (int32 h = 0; h < heightNum - 2; ++h)
	{
		for (int32 w = 0; w < widthNum; ++w)
		{
			MassPoint* origin = &_massPointList[w + h * widthNum];
			MassPoint* distLower = &_massPointList[w + (h + 2) * widthNum];

			float restLengthDistRight = glm::distance(origin->position, distLower->position);

			_springList.push_back({ SpringType::Flexion, restLengthDistRight, origin, distLower });
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

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	_test = test;
}

PlaneCloth::~PlaneCloth(void)
{
	glDeleteVertexArrays(1, &_vao);
	glDeleteBuffers(1, &_vbo);
	glDeleteBuffers(1, &_ebo);
}

void PlaneCloth::update(float deltaTime)
{
	applyInternalForces();
	applyExternalForces();
	updateMassPointState(deltaTime);
	solveCollision();
	updateMassPointNormal();
}

void PlaneCloth::render(Shader& shader)
{
	glm::mat4 worldMat = glm::translate(glm::mat4(1.0f), _position);
	shader.setUniformVec3("Ka", _materialAmbient);
	shader.setUniformVec3("Ke", _materialDiffuse);
	shader.setUniformVec3("Ks", _materialSpecular);
	shader.setUniformFloat("sh", _materialShininess);
	shader.setUniformMat4("worldMat", worldMat);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glBindVertexArray(_vao);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, _massPointList.size() * sizeof(MassPoint), _massPointList.data());
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);

	glLineWidth(1.0f);
	glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, NULL);
}

void PlaneCloth::applyInternalForces(void)
{
	static const float stiffnessList[3] = {
		2000.0f, 
		2000.0f,
		3000.0f
	};

	static const float dampingCoefficient = 18.0f;

	for (Spring& spring : _springList)
	{
		MassPoint* left = spring.left;
		MassPoint* right = spring.right;

		glm::vec3 posDifference = left->position - right->position;
		glm::vec3 velDfference = left->velocity - right->velocity;

		float stiffness = stiffnessList[static_cast<uint32>(spring.type)];
		float dist = glm::length(posDifference);

		glm::vec3 springForce = -stiffness * (dist - spring.restLength) * glm::normalize(posDifference);
		glm::vec3 dampingForce = -dampingCoefficient * velDfference;

		left->netForce += (springForce + dampingForce);
		right->netForce -= (springForce + dampingForce);
	}
}

void PlaneCloth::applyExternalForces(void)
{
	// Viscosity, Friction...
	for (MassPoint& massPoint : _massPointList)
		massPoint.netForce += glm::vec3(0.0f, -9.81f, 0.0f) * massPoint.mass;
}

void PlaneCloth::updateMassPointState(float deltaTime)
{
	for (MassPoint& massPoint : _massPointList)
	{
		glm::vec3 newVelocity = massPoint.velocity + deltaTime * massPoint.netForce / massPoint.mass;
		glm::vec3 newPosition = massPoint.position + deltaTime * massPoint.velocity;

		massPoint.velocity = newVelocity;
		massPoint.position = newPosition;
		massPoint.netForce = glm::vec3(0.0f, 0.0f, 0.0f);
	}
}

void PlaneCloth::solveCollision(void)
{
	static glm::vec3 center = glm::vec3(15.0f, -13.0f, 15.0f);
	static float radius = 5.0f;
	for (MassPoint& massPoint : _massPointList)
	{
		glm::vec3 position = massPoint.position;
		massPoint.color = glm::vec3(0.9f, 0.9f, 0.9f);

		if (position.y < -17.0f)
		{
			massPoint.position.y = -17.0f;
			massPoint.velocity.y = 0.0f;
			continue;
		}

		if (_test)
		{
			float distance = glm::distance(position, center);
			if (distance < radius + 0.2)
			{
				massPoint.velocity = glm::vec3(0.0f);
				massPoint.color = glm::vec3(1.0f, 0.0f, 0.0f);
			}
		}
		else
			if ((position.y < -7.8f) && (position.x < 25.2f && position.x > 14.8f) && (position.z < 20.2f && position.z > 9.8f))
			{
				std::vector<float> depth(5);

				float frontDepth = abs(position.x - 14.8f);
				float backDepth = abs(position.x - 25.2f);
				float leftDepth = abs(position.z - 9.8f);
				float rightDepth = abs(position.z - 20.2f);
				float upperDepth = abs(position.y + 7.8f);

				depth[0] = frontDepth;
				depth[1] = backDepth;
				depth[2] = leftDepth;
				depth[3] = rightDepth;
				depth[4] = upperDepth;

				std::sort(depth.begin(), depth.end());

				if (depth[0] == upperDepth)
					massPoint.velocity.y = 0.0f;
				else if (depth[0] == frontDepth || depth[0] == backDepth)
					massPoint.velocity.x = 0.0f;
				else
					massPoint.velocity.z = 0.0f;

				massPoint.color = glm::vec3(1.0f, 0.0f, 0.0f);
			}
	}
}

void PlaneCloth::updateMassPointNormal(void)
{
	std::vector<std::vector<glm::vec3>> normals(_massPointList.size());
	for (int32 i = 0; i < _indices.size(); i += 6) {

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

	for (int32 i = 0; i < normals.size(); i++) {
		glm::vec3 avg{ 0.0f, 0.0f, 0.0f };

		for (int32 j = 0; j < normals[i].size(); j++)
			avg += normals[i][j];

		avg = glm::normalize(avg);
		_massPointList[i].normal = avg;
	}
}
