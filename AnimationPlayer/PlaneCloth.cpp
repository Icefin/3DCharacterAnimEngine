#include <algorithm>

#include "PlaneCloth.h"

PlaneCloth::PlaneCloth(glm::vec3 position, uint32 width, uint32 height, uint32 widthNum, uint32 heightNum)
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
			newPoint.velocity = glm::vec3(0.0f, -0.1f, 0.0f);
			newPoint.netForce = glm::vec3(0.0f, 0.0f, 0.0f);
			newPoint.color = glm::vec3(0.9f, 0.9f, 0.9f);

			_massPointList[w + h * widthNum] = newPoint;
		}
	}

	// StructuralSpring Initialize
	// Recheck edge cases... ex) Right Most, Lower Most....
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

	glGenBuffers(1, &_ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(uint32), _indices.data(), GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

PlaneCloth::~PlaneCloth(void)
{
	glDeleteVertexArrays(1, &_vao);
	glDeleteBuffers(1, &_vbo);
	glDeleteBuffers(1, &_ebo);
}

#include <stdio.h>
void PlaneCloth::update(Shader& shader, float deltaTime)
{
	applyInternalForce();
	applyExternalForce();
	updateMassPointState(deltaTime / 600);
	solveCollision();
	render(shader);
	
	printf("MassPoint Position : (%f, %f, %f)\n", _massPointList[1230].position.x, _massPointList[1230].position.y, _massPointList[1230].position.z);
}

void PlaneCloth::applyInternalForce(void)
{
	static const uint32 stiffnessList[3] = {
		10, 
		10,
		12
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

		glm::vec3 springForce = -stiffness * (spring.restLength - dist) * glm::normalize(posDifference);
		glm::vec3 dampingForce = -dampingCoefficient * velDfference;

		left->netForce += (springForce + dampingForce);
		right->netForce -= (springForce + dampingForce);
	}
}

void PlaneCloth::applyExternalForce(void)
{
	// Viscosity
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
		massPoint.netForce = glm::vec3(0);
	}
}

void PlaneCloth::solveCollision(void)
{
	for (MassPoint& massPoint : _massPointList)
	{
		glm::vec3 position = massPoint.position;
		massPoint.color = glm::vec3(0.9f, 0.9f, 0.9f);

		if (position.y < -17.0f)
		{
			massPoint.position.y = -17.0f;
			massPoint.velocity.y = 0.0f;
		}

		if ((position.y < -8.2f) && (position.x < 20.2f && position.x > 10.2f) && (position.z < 20.2f && position.z > 10.2f))
		{
			std::vector<float> depth(5);

			float frontDepth = abs(position.x - 10.2f);
			float backDepth = abs(position.x - 20.2f);
			float leftDepth = abs(position.z - 10.2f);
			float rightDepth = abs(position.z - 20.2f);
			float upperDepth = abs(position.y + 8.2f);

			depth[0] = frontDepth;
			depth[1] = backDepth;
			depth[2] = leftDepth;
			depth[3] = rightDepth;
			depth[4] = upperDepth;

			std::sort(depth.begin(), depth.end());

			if (depth[0] == upperDepth)
			{
				massPoint.position.y = -8.2f;
				massPoint.velocity.y = 0.0f;
			}
			else if (depth[0] == frontDepth)
			{
				massPoint.position.x = 10.2f;
				massPoint.velocity.x = 0.0f;
			}
			else if (depth[0] == backDepth)
			{
				massPoint.position.x = 20.2f;
				massPoint.velocity.x = 0.0f;
			}
			else if (depth[0] == leftDepth)
			{
				massPoint.position.z = 10.2f;
				massPoint.velocity.z = 0.0f;
			}
			else
			{
				massPoint.position.z = 20.2f;
				massPoint.velocity.z = 0.0f;
			}

			massPoint.color = glm::vec3(1.0f, 0.0f, 0.0f);
		}
	}
}

void PlaneCloth::render(Shader& shader)
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glBindVertexArray(_vao);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, _massPointList.size() * sizeof(MassPoint), _massPointList.data());
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);

	glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), _position);
	shader.setUniformMat4("model", modelMatrix);
	glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, NULL);
}