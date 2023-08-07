#include "Geometry3D.h"

#define EPSILON 0.001f

namespace pa
{
#pragma region Point
	bool isPointInside(const Point& point, const Line& line)
	{
		Point closest = getClosestPoint(point, line);

		float distance = glm::length(closest - point);
		return (distance < EPSILON);
	}

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

	bool isPointInside(const Point& point, const Plane& plane)
	{
		float distance = glm::abs(glm::dot(point, plane.normal));

		return (distance - plane.distance < EPSILON);
	}

	bool isPointInside(const Point& point, const Sphere& sphere)
	{
		// Square Root Replacement
		float distance = glm::length(point - sphere.position);

		return distance < sphere.radius;
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
		glm::vec3 lineDirection = line.p2 - line.p1;
		//Recheck Here
		float t = glm::dot(point - line.p1, lineDirection) / glm::length(lineDirection);

		t = fmaxf(t, 0.0f);
		t = fminf(t, 1.0f);
		return line.p1 + lineDirection * t;
	}

	Point getClosestPoint(const Point& point, const Triangle& triangle)
	{
		Plane plane = makePlaneFromTriangle(triangle);

		Point closest = getClosestPoint(point, plane);
		if (isPointInside(closest, triangle) == true)
			return closest;

		Point c1 = getClosestPoint(point, Line(triangle.p1, triangle.p2));
		Point c2 = getClosestPoint(point, Line(triangle.p1, triangle.p2));
		Point c3 = getClosestPoint(point, Line(triangle.p1, triangle.p2));

		float magSq1 = glm::length(point - c1);
		float magSq2 = glm::length(point - c2);
		float magSq3 = glm::length(point - c3);

		if (magSq1 < magSq2 && magSq1 < magSq3)
			return c1;
		else if (magSq2 < magSq1 && magSq2 < magSq3)
			return c2;
		return c3;
	}

	Point getClosestPoint(const Point& point, const Plane& plane)
	{
		//Recheck Here
		float dot = glm::dot(plane.normal, point);
		float distance = dot - plane.distance;

		return (point - plane.normal * distance);
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



#pragma region Line
	float getLineLength(const Line& line)
	{
		return glm::length(line.p1 - line.p2);
	}
#pragma endregion



#pragma region Triangle
	bool isTriangleTriangleCollision(const Triangle& t1, const Triangle& t2)
	{
		__noop;
	}

	bool isTrianglePlaneCollision(const Triangle& triangle, const Plane& plane)
	{
		float d1 = planeEquation(triangle.p1, plane);
		float d2 = planeEquation(triangle.p2, plane);
		float d3 = planeEquation(triangle.p3, plane);

		if (glm::abs(d1) < EPSILON && glm::abs(d2) < EPSILON && glm::abs(d3) < EPSILON)
			return true;
		if (d1 > 0.0f && d2 > 0.0f && d3 > 0.0f)
			return false;
		if (d1 < 0.0f && d2 < 0.0f && d3 < 0.0f)
			return false;
		return true;
	}

	bool isTriangleSphereCollision(const Triangle& triangle, const Sphere& sphere)
	{
		Point closest = getClosestPoint(sphere.position, triangle);
		// Square Root Replacement
		float length = glm::length(closest - sphere.position);
		return (length <= sphere.radius);
	}

	bool isTriangleAABBCollision(const Triangle& triangle, const AABB& aabb)
	{
		__noop;
	}

	bool isTriangleOBBCollision(const Triangle& triangle, const OBB& obb)
	{
		__noop;
	}

	bool isTriangleCylinderCollision(const Triangle& triangle, const Cylinder& cylinder)
	{
		__noop;
	}
#pragma endregion



#pragma region Plane
	float planeEquation(const Point& point, const Plane& plane)
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
	void resetRaycastInfo(RaycastInfo* outInfo)
	{
		if (outInfo != nullptr)
		{
			outInfo->hitPoint = glm::vec3(0.0f, 0.0f, 0.0f);
			outInfo->normal = glm::vec3(0.0f, 0.0f, 1.0f);
			outInfo->rayTime = -1.0f;
			outInfo->isHit = false;
		}
	}

	glm::vec3 getBarycentricCoordinate(const Point& point, const Triangle& triangle)
	{
		glm::vec3 ap = point - triangle.p1;
		glm::vec3 bp = point - triangle.p2;
		glm::vec3 cp = point - triangle.p3;

		glm::vec3 ab = triangle.p2 - triangle.p1;
		glm::vec3 ac = triangle.p3 - triangle.p1;
		glm::vec3 bc = triangle.p3 - triangle.p2;

		__noop;
	}

	float raycast(const Ray& ray, const Triangle& triangle)
	{
		Plane plane = makePlaneFromTriangle(triangle);
		float t = raycast(ray, plane);
		if (t < 0.0f)
			return t;

		Point rayPoint = ray.origin + ray.direction * t;

		glm::vec3 barycentric = getBarycentricCoordinate(rayPoint, triangle);
		if (barycentric.x >= 0.0f && barycentric.x <= 1.0f &&
			barycentric.y >= 0.0f && barycentric.y <= 1.0f &&
			barycentric.z >= 0.0f && barycentric.z <= 1.0f)
			return t;

		return -1;
	}

	float raycast(const Ray& ray, const Plane& plane)
	{
		float nd = glm::dot(ray.direction, plane.normal);
		float pn = glm::dot(ray.origin, plane.normal);

		if (nd >= 0.0f)
			return -1;

		float t = (plane.distance - pn) / nd;

		if (t > 0.0f)
			return t;

		return -1;
	}

	bool raycast(const Ray& ray, const Sphere& sphere, RaycastInfo* outInfo)
	{
		resetRaycastInfo(outInfo);

		glm::vec3 toSphere = sphere.position - ray.origin;
		float dot = glm::dot(toSphere, ray.direction);



	}

	bool raycast(const Ray& ray, const AABB& aabb, RaycastInfo* outInfo)
	{
		resetRaycastInfo(outInfo);
	}

	bool raycast(const Ray& ray, const OBB& obb, RaycastInfo* outInfo)
	{
		resetRaycastInfo(outInfo);
	}

	float raycast(const Ray& ray, const Mesh& mesh)
	{
		int32 n = mesh.numTriangles;

		for (int32 i = 0; i < n; ++i)
		{
			float t = raycast(ray, mesh.triangles[i]);
			if (t >= 0.0f)
				return t;
		}
		return -1;
	}
#pragma endregion



#pragma region AABB
	glm::vec3 getMinFromAABB(const AABB& aabb) 
	{
		glm::vec3 p1 = aabb.position + aabb.size;
		glm::vec3 p2 = aabb.position - aabb.size;
		return glm::vec3(fminf(p1.x, p2.x),
			fminf(p1.y, p2.y),
			fminf(p1.z, p2.z));
	}

	glm::vec3 getMaxFromAABB(const AABB& aabb)
	{
		glm::vec3 p1 = aabb.position + aabb.size;
		glm::vec3 p2 = aabb.position - aabb.size;
		return glm::vec3(fmaxf(p1.x, p2.x),
			fmaxf(p1.y, p2.y),
			fmaxf(p1.z, p2.z));
	}

	AABB makeAABBFromMinMax(const glm::vec3& min, const glm::vec3& max) 
	{
		return AABB((min + max) * 0.5f, (max - min) * 0.5f);
	}

	bool isAABBAABBCollision(const AABB& a1, const AABB& a2)
	{
		Point a1Min = getMinFromAABB(a1);
		Point a1Max = getMaxFromAABB(a1);

		Point a2Min = getMinFromAABB(a2);
		Point a2Max = getMaxFromAABB(a2);

		return (a1Min.x <= a2Max.x && a1Max.x >= a2Min.x) &&
			(a1Min.y <= a2Max.y && a1Max.y >= a2Min.y) &&
			(a1Min.z <= a2Max.z && a1Max.z >= a2Min.z);
	}

	bool isAABBSphereCollision(const AABB& aabb, const Sphere& sphere)
	{
		return isSphereAABBCollision(sphere, aabb);
	}

	bool isAABBOBBCollision(const AABB& aabb, const OBB& obb)
	{
		__noop;
	}
#pragma endregion



#pragma region OBB
	bool isOBBOBBCollision(const OBB& o1, const OBB& o2)
	{
		__noop;
	}

	bool isOBBSphereCollision(const OBB& obb, const Sphere& sphere)
	{
		return isSphereOBBCollision(sphere, obb);
	}

	bool isOBBAABBCollision(const OBB& obb, const AABB& aabb)
	{
		return isAABBOBBCollision(aabb, obb);
	}
#pragma endregion



#pragma region Sphere
	bool isSphereSphereCollision(const Sphere& s1, const Sphere& s2)
	{
		//Square Root Replacement
		float distance = glm::length(s1.position - s2.position);
		float radiusSum = s1.radius + s2.radius;

		return distance < radiusSum;
	}

	bool isSphereAABBCollision(const Sphere& sphere, const AABB& aabb)
	{
		Point closestPoint = getClosestPoint(sphere.position, aabb);

		float distance = glm::length(sphere.position - closestPoint);
		return distance < sphere.radius;
	}

	bool isSphereOBBCollision(const Sphere& sphere, const OBB& obb)
	{
		Point closestPoint = getClosestPoint(sphere.position, obb);

		float distance = glm::length(sphere.position - closestPoint);
		return (distance < sphere.radius);
	}
#pragma endregion



#pragma region Cylinder

#pragma endregion



#pragma region Mesh
	bool isMeshPlaneCollision(const Mesh& mesh, const Plane& plane)
	{
		int32 n = mesh.numTriangles;

		for (int32 i = 0; i < n; ++i)
		{
			if (isTrianglePlaneCollision(mesh.triangles[i], plane) == true)
				return true;
		}
		return false;
	}

	bool isMeshSphereCollision(const Mesh& mesh, const Sphere& sphere)
	{
		int32 n = mesh.numTriangles;

		for (int32 i = 0; i < n; ++i)
		{
			if (isTriangleSphereCollision(mesh.triangles[i], sphere) == true)
				return true;
		}
		return false;
	}

	bool isMeshAABBCollision(const Mesh& mesh, const AABB& aabb)
	{
		int32 n = mesh.numTriangles;

		for (int32 i = 0; i < n; ++i)
		{
			if (isTriangleAABBCollision(mesh.triangles[i], aabb) == true)
				return true;
		}
		return false;
	}

	bool isMeshOBBCollision(const Mesh& mesh, const OBB& obb)
	{
		int32 n = mesh.numTriangles;

		for (int32 i = 0; i < n; ++i)
		{
			if (isTriangleOBBCollision(mesh.triangles[i], obb) == true)
				return true;
		}
		return false;
	}

	bool isMeshCylinderCollision(const Mesh& mesh, const Cylinder& cylinder)
	{
		int32 n = mesh.numTriangles;

		for (int32 i = 0; i < n; ++i)
		{
			if (isTriangleCylinderCollision(mesh.triangles[i], cylinder) == true)
				return true;
		}
		return false;
	}
#pragma endregion



#pragma region Frustum
	Point findIntersectionPoint(const Plane& p1, const Plane& p2, const Plane& p3)
	{
		__noop;
	}

	void getCorners(const Frustum& f, glm::vec3* outCorners)
	{
		__noop;
	}
#pragma endregion



#pragma region Model
	void Model::setMesh(Mesh* mesh)
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

	OBB Model::getOBB(void)
	{
		OBB result;

		glm::mat4 worldMatrix = getWorldMatrix();
		__noop;
	}

	glm::mat4 Model::getWorldMatrix(void) const
	{
		glm::mat4 translation = glm::translate(glm::mat4(1.0f), position);
		//glm::mat4 rotation = glm::rotate();

		glm::mat4 localMatrix = /*rotation * */translation;

		if (parent != nullptr)
		{
			glm::mat4 parentMatrix = parent->getWorldMatrix();
			return localMatrix * parentMatrix;
		}
		return localMatrix;
	}
#pragma endregion
}