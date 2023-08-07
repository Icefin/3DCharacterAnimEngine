#include "Geometry3D.h"

#pragma region Point
bool isPointInside(const Point& point, const Triangle& triangle)
{
	glm::vec3 a = triangle.p1 - point;
	glm::vec3 b = triangle.p2 - point;
	glm::vec3 c = triangle.p3 - point;

	glm::vec3 u = glm::cross(b, c);
	glm::vec3 v = glm::cross(c, a);
	glm::vec3 w = glm::cross(a, b);

	if (glm::dot(u, v) < 0.0f)
		return false;
	if (glm::dot(u, w) < 0.0f)
		return false;
	return true;
}

bool isPointInside(const Point& point, const Sphere& sphere)
{
	// Square Root Replacement
	float dist = glm::length(point - sphere.position);

	return dist < sphere.radius;
}

bool isPointInside(const Point& point, const AABB& aabb)
{
	glm::vec3 min = getMinFromAABB(aabb);
	glm::vec3 max = getMaxFromAABB(aabb);

	if (point.x < min.x || point.y < min.y || point.z < min.z)
		return false;
	if (point.x > max.x || point.y > max.y || point.z > max.z)
		return false;
	return true;
}

bool isPointInside(const Point& point, const OBB& obb)
{
	__noop;
}

bool isPointInside(const Point& point, const Cylinder& cylinder)
{
	__noop;
}

Point getClosestPoint(const Point& point, const Line& line)
{
	__noop;
}

Point getClosestPoint(const Point& point, const Triangle& triangle)
{
	Plane plane = makePlaneFromTriangle(triangle);

	Point closest = getClosestPoint(point, plane);
	if (isPointInside(closest, triangle) == true)
		return closest;


}

Point getClosestPoint(const Point& point, const Plane& plane)
{
	__noop;
}

Point getClosestPoint(const Point& point, const Sphere& sphere)
{
	glm::vec3 sphereToPoint = glm::normalize(point - sphere.position) * sphere.radius;

	return sphere.position + sphereToPoint;
}

Point getClosestPoint(const Point& point, const AABB& aabb)
{
	Point result = point;
	Point min = getMinFromAABB(aabb);
	Point max = getMaxFromAABB(aabb);
	
	result.x = (result.x < min.x) ? min.x : result.x;
	result.y = (result.y < min.x) ? min.y : result.y;
	result.z = (result.z < min.x) ? min.z : result.z;
	
	result.x = (result.x > max.x) ? max.x : result.x;
	result.y = (result.y > max.x) ? max.y : result.y;
	result.z = (result.z > max.x) ? max.z : result.z;
	return result;
}

Point getClosestPoint(const Point& point, const OBB& obb)
{
	__noop;
}

Point getClosestPoint(const Point& point, const Cylinder& cylinder)
{
	__noop;
}
#pragma endregion



#pragma region Triangle

#pragma endregion



#pragma region Plane
float getPlaneEquation(const Point& point, const Plane& plane)
{
	return (glm::dot(point, plane.normal) - plane.distance);
}

Plane makePlaneFromTriangle(const Triangle& triangle)
{
	Plane result;

	result.normal = glm::normalize(glm::cross(triangle.p2 - triangle.p1, triangle.p3 - triangle.p1));
	result.distance = glm::dot(result.normal, triangle.p1);
	return result;
}
#pragma endregion



#pragma region Ray

#pragma endregion



#pragma region AABB
glm::vec3 getMinFromAABB(const AABB& aabb) {
	glm::vec3 p1 = aabb.position + aabb.size;
	glm::vec3 p2 = aabb.position - aabb.size;
	return glm::vec3(fminf(p1.x, p2.x),
		fminf(p1.y, p2.y),
		fminf(p1.z, p2.z));
}

glm::vec3 getMaxFromAABB(const AABB& aabb) {
	glm::vec3 p1 = aabb.position + aabb.size;
	glm::vec3 p2 = aabb.position - aabb.size;
	return glm::vec3(fmaxf(p1.x, p2.x),
		fmaxf(p1.y, p2.y),
		fmaxf(p1.z, p2.z));
}

AABB makeAABBFromMinMax(const glm::vec3& min, const glm::vec3& max) {
	return AABB((min + max) * 0.5f, (max - min) * 0.5f);
}
#pragma endregion AABB



#pragma region OBB

#pragma endregion



#pragma region Sphere

#pragma endregion



#pragma region Mesh
void Model::SetMesh(Mesh* mesh)
{
	_mesh = mesh;

	if (_mesh != 0)
	{
		glm::vec3 min = _mesh->points[0];
		glm::vec3 max = _mesh->points[0];

		for (int i = 1; i < mesh->numTriangles * 3; ++i) {
			min.x = fminf(mesh->points[i].x, min.x);
			min.y = fminf(mesh->points[i].y, min.y);
			min.z = fminf(mesh->points[i].z, min.z);
			max.x = fmaxf(mesh->points[i].x, max.x);
			max.y = fmaxf(mesh->points[i].y, max.y);
			max.z = fmaxf(mesh->points[i].z, max.z);
		}
		_bounds = makeAABBFromMinMax(min, max);
	}
}
#pragma endregion



#pragma region CollisionDetection
bool checkSphereSphereCollision(const Sphere& s1, const Sphere& s2)
{
	//Square Root Replacement
	float dist = glm::length(s1.position - s2.position);
	float radiusSum = s1.radius + s2.radius;

	return dist < radiusSum;
}

bool checkAABBAABBCollision(const AABB& a1, const AABB& a2)
{
	Point a1Min = getMinFromAABB(a1);
	Point a1Max = getMaxFromAABB(a1);

	Point a2Min = getMinFromAABB(a2);
	Point a2Max = getMaxFromAABB(a2);

	return (a1Min.x <= a2Max.x && a1Max.x >= a2Min.x) &&
		(a1Min.y <= a2Max.y && a1Max.y >= a2Min.y) &&
		(a1Min.z <= a2Max.z && a1Max.z >= a2Min.z);
}

bool checkOBBOBBCollision(const OBB& o1, const OBB& o2)
{
	__noop;
}

bool checkSphereAABBCollision(const Sphere& sphere, const AABB& aabb)
{
	Point closestPoint = getClosestPoint(sphere.position, aabb);

	float dist = glm::length(sphere.position - closestPoint);
	return dist < sphere.radius;
}

bool checkAABBSphereCollision(const AABB& aabb, const Sphere& sphere)
{
	return checkSphereAABBCollision(sphere, aabb);
}

bool checkSphereOBBCollision(const Sphere& sphere, const OBB& obb)
{
	Point closestPoint = getClosestPoint(sphere.position, obb);

	float dist = glm::length(sphere.position - closestPoint);
	return dist < sphere.radius;
}

bool checkOBBSphereCollision(const OBB& obb, const Sphere& sphere)
{
	return checkSphereOBBCollision(sphere, obb);
}

bool checkAABBOBBCollision(const AABB& aabb, const OBB& obb)
{
	__noop;
}

bool checkOBBAABBCollision(const OBB& obb, const AABB& aabb)
{
	return checkAABBOBBCollision(aabb, obb);
}
#pragma region