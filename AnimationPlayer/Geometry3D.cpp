#include "Geometry3D.h"

#define EPSILON 0.00001f

namespace pa
{
#pragma region Point
	bool isPointInside(const Point& point, const Line& line)
	{
		Point closest = findClosestPoint(point, line);

		float squareLength = calculateSquareLength(Line(closest, point));
		return (squareLength < EPSILON);
	}
	bool isPointInside(const Point& point, const Triangle& triangle)
	{
		glm::vec3 pa = triangle.p1 - point;
		glm::vec3 pb = triangle.p2 - point;
		glm::vec3 pc = triangle.p3 - point;

		glm::vec3 u = glm::cross(pa, pc);
		glm::vec3 v = glm::cross(pc, pa);
		glm::vec3 w = glm::cross(pa, pb);

		if (glm::dot(u, v) < 0.0f)
			return false;
		if (glm::dot(u, w) < 0.0f)
			return false;
		return true;
	}
	bool isPointInside(const Point& point, const Plane& plane)
	{
		float distance = glm::dot(point, plane.normal);

		return (distance - plane.distance < EPSILON);
	}
	bool isPointInside(const Point& point, const Ray& ray)
	{
		if (point == ray.origin)
			return true;

		glm::vec3 rayToPoint = glm::normalize(point - ray.origin);
		float similarity = glm::dot(rayToPoint, ray.direction);
		return (glm::abs(similarity - 1.0f) < EPSILON);
	}
	bool isPointInside(const Point& point, const Sphere& sphere)
	{
		float squareLength = calculateSquareLength(Line(point, sphere.position));

		return (squareLength < sphere.radius * sphere.radius);
	}
	bool isPointInside(const Point& point, const AABB& aabb)
	{
		Point min = getMinFromAABB(aabb);
		Point max = getMaxFromAABB(aabb);

		if (point.x < min.x || point.y < min.y || point.z < min.z)
			return false;
		if (point.x > max.x || point.y > max.y || point.z > max.z)
			return false;
		return true;
	}
	bool isPointInside(const Point& point, const OBB& obb)
	{
		glm::vec3 obbToPoint = point - obb.position;

		glm::mat3 rotation = glm::mat3(obb.orientation);
		for (int32 i = 0; i < 3; i++)
		{
			//Recheck Here
			glm::vec3 basis = glm::vec3(rotation[i][0], rotation[i][1], rotation[i][2]);
			float distance = glm::dot(obbToPoint, basis);

			if (distance > obb.size[i])
				return false;
			if (distance < -obb.size[i])
				return false;
		}
		return true;
	}

	Point findClosestPoint(const Point& point, const Line& line)
	{
		glm::vec3 lineVector = line.p2 - line.p1;
		float t = glm::dot(point - line.p1, lineVector) / glm::dot(lineVector, lineVector);

		t = fmaxf(t, 0.0f);
		t = fminf(t, 1.0f);
		return line.p1 + lineVector * t;
	}
	Point findClosestPoint(const Point& point, const Triangle& triangle)
	{
		Plane plane = makePlaneFromTriangle(triangle);

		Point closest = findClosestPoint(point, plane);
		if (isPointInside(closest, triangle) == true)
			return closest;

		Point c1 = findClosestPoint(point, Line(triangle.p1, triangle.p2));
		Point c2 = findClosestPoint(point, Line(triangle.p1, triangle.p2));
		Point c3 = findClosestPoint(point, Line(triangle.p1, triangle.p2));

		float squareLength1 = calculateSquareLength(Line(point, c1));
		float squareLength2 = calculateSquareLength(Line(point, c2));
		float squareLength3 = calculateSquareLength(Line(point, c3));

		if (squareLength1 < squareLength2 && squareLength1 < squareLength3)
			return c1;
		else if (squareLength2 < squareLength1 && squareLength2 < squareLength3)
			return c2;
		return c3;
	}
	Point findClosestPoint(const Point& point, const Plane& plane)
	{
		float dot = glm::dot(plane.normal, point);
		float distance = dot - plane.distance;

		return (point - plane.normal * distance);
	}
	Point findClosestPoint(const Point& point, const Ray& ray)
	{
		glm::vec3 rayToPoint = point - ray.origin;

		float t = glm::dot(rayToPoint, ray.direction);
		t = fmaxf(t, 0.0f);
		
		return ray.origin + ray.direction * t;
	}
	Point findClosestPoint(const Point& point, const Sphere& sphere)
	{
		glm::vec3 sphereToPoint = glm::normalize(point - sphere.position) * sphere.radius;

		return sphere.position + sphereToPoint;
	}
	Point findClosestPoint(const Point& point, const AABB& aabb)
	{
		Point closest = point;
		Point min = getMinFromAABB(aabb);
		Point max = getMaxFromAABB(aabb);

		closest.x = (closest.x < min.x) ? min.x : closest.x;
		closest.y = (closest.y < min.x) ? min.y : closest.y;
		closest.z = (closest.z < min.x) ? min.z : closest.z;

		closest.x = (closest.x > max.x) ? max.x : closest.x;
		closest.y = (closest.y > max.x) ? max.y : closest.y;
		closest.z = (closest.z > max.x) ? max.z : closest.z;

		//Recheck Here
		return closest;
	}
	Point findClosestPoint(const Point& point, const OBB& obb)
	{
		Point closest = obb.position;
		glm::vec3 obbToPoint = point - obb.position;

		glm::mat3 rotation = glm::mat3(obb.orientation);
		for (int32 i = 0; i < 3; i++)
		{
			//Recheck Here
			glm::vec3 basis = glm::vec3(rotation[i][0], rotation[i][1], rotation[i][2]);
			float distance = glm::dot(obbToPoint, basis);

			if (distance > obb.size[i])
				distance = obb.size[i];
			if (distance < -obb.size[i])
				distance = -obb.size[i];

			closest = closest + (basis * distance);
		}
		return closest;
	}
#pragma endregion



#pragma region Line
	float calculateSquareLength(const Line& line)
	{
		glm::vec3 direction = line.p2 - line.p1;
		return glm::dot(direction, direction);
	}

	bool isIntersection(const Line& line, const Triangle& triangle)
	{
		Ray ray;
		ray.origin = line.p1;
		ray.direction = glm::normalize(line.p2 - line.p1);

		RaycastInfo raycastInfo;
		raycast(ray, triangle, &raycastInfo);

		if (raycastInfo.isHit == false)
			return false;
		
		float squareLength = calculateSquareLength(line);
		float squareRayTime = raycastInfo.rayTime * raycastInfo.rayTime;

		return (squareRayTime <= squareLength);
	}
	bool isIntersection(const Line& line, const Plane& plane)
	{
		glm::vec3 direction = line.p2 - line.p1;

		float nA = glm::dot(plane.normal, line.p1);
		float nBA = glm::dot(plane.normal, direction);

		float t = (plane.distance - nA) / nBA;

		return (t >= 0.0f && t <= 1.0f);
	}
	bool isIntersection(const Line& line, const Sphere& sphere)
	{
		Point closest = findClosestPoint(sphere.position, line);

		float squareLength = calculateSquareLength(Line(sphere.position, closest));
		return (squareLength < sphere.radius * sphere.radius);
	}
	bool isIntersection(const Line& line, const AABB& aabb)
	{
		Ray ray;
		ray.origin = line.p1;
		ray.direction = glm::normalize(line.p2 - line.p1);

		RaycastInfo raycastInfo;
		raycast(ray, aabb, &raycastInfo);

		float squareLength = calculateSquareLength(line);

		return (raycastInfo.isHit && raycastInfo.rayTime * raycastInfo.rayTime < squareLength);
	}
	bool isIntersection(const Line& line, const OBB& obb)
	{
		Ray ray;
		ray.origin = line.p1;
		ray.direction = glm::normalize(line.p2 - line.p1);

		RaycastInfo raycastInfo;
		raycast(ray, obb, &raycastInfo);

		float squareLength = calculateSquareLength(line);

		return (raycastInfo.isHit && raycastInfo.rayTime * raycastInfo.rayTime < squareLength);
	}

	/*
	bool isIntersection(const Line& line, const Mesh& mesh)
	{
		__noop;
		return true;
	}
	bool isIntersection(const Line& line, const Model& model)
	{
		glm::mat4 worldMatrix = model.getWorldMatrix();
		glm::mat4 invWorld = glm::inverse(worldMatrix);

		Line localLine;
		localLine.p1 = glm::vec3(invWorld * glm::vec4(line.p1, 1.0f));
		localLine.p2 = glm::vec3(invWorld * glm::vec4(line.p2, 1.0f));

		if (model.getMesh() != nullptr)
			return isIntersection(localLine, *(model.getMesh()));
		return false;
	}
	*/
#pragma endregion



#pragma region Interval
	Interval findInterval(const Triangle& triangle, const glm::vec3& axis)
	{
		Interval interval;

		float p1 = glm::dot(axis, triangle.p1);
		float p2 = glm::dot(axis, triangle.p2);
		float p3 = glm::dot(axis, triangle.p3);
		
		interval.min = fminf(p1, fminf(p2, p3));
		interval.max = fmaxf(p1, fmaxf(p2, p3));

		return interval;
	}
	Interval findInterval(const AABB& aabb, const glm::vec3& axis)
	{
		Interval interval;

		Point min = getMinFromAABB(aabb);
		Point max = getMaxFromAABB(aabb);

		Point points[8] = {
			Point(min.x, max.y, max.z),
			Point(min.x, max.y, min.z),
			Point(min.x, min.y, max.z),
			Point(min.x, min.y, min.z),
			Point(max.x, max.y, max.z),
			Point(max.x, max.y, min.z),
			Point(max.x, min.y, max.z),
			Point(max.x, min.y, min.z)
		};

		interval.min = interval.max = glm::dot(axis, points[0]);
		for (int32 i = 1; i < 8; ++i)
		{
			float projection = glm::dot(axis, points[i]);
			interval.min = (projection < interval.min) ? projection : interval.min;
			interval.max = (projection > interval.max) ? projection : interval.max;
		}

		return interval;
	}
	Interval findInterval(const OBB& obb, const glm::vec3& axis)
	{
		Interval interval;

		Point center = obb.position;
		glm::vec3 halfSide = obb.size;

		//Recheck Here
		glm::mat3 rotation = glm::mat3(obb.orientation);
		glm::vec3 basis[3] = {
			glm::vec3(rotation[0][0], rotation[0][1], rotation[0][2]),
			glm::vec3(rotation[1][0], rotation[1][1], rotation[1][2]),
			glm::vec3(rotation[2][0], rotation[2][1], rotation[2][2])
		};

		Point points[8] = {
			center + basis[0] * halfSide[0] + basis[1] * halfSide[1] + basis[2] * halfSide[2],
			center + basis[0] * halfSide[0] + basis[1] * halfSide[1] - basis[2] * halfSide[2],
			center + basis[0] * halfSide[0] - basis[1] * halfSide[1] + basis[2] * halfSide[2],
			center + basis[0] * halfSide[0] - basis[1] * halfSide[1] - basis[2] * halfSide[2],
			center - basis[0] * halfSide[0] + basis[1] * halfSide[1] + basis[2] * halfSide[2],
			center - basis[0] * halfSide[0] + basis[1] * halfSide[1] - basis[2] * halfSide[2],
			center - basis[0] * halfSide[0] - basis[1] * halfSide[1] + basis[2] * halfSide[2],
			center - basis[0] * halfSide[0] - basis[1] * halfSide[1] - basis[2] * halfSide[2]
		};

		interval.min = interval.max = glm::dot(axis, points[0]);
		for (int32 i = 1; i < 8; ++i)
		{
			float projection = glm::dot(axis, points[i]);
			interval.min = (projection < interval.min) ? projection : interval.min;
			interval.max = (projection > interval.max) ? projection : interval.max;
		}

		return interval;
	}

	bool isOverlapOnAxis(const Triangle& t1, const Triangle& t2, const glm::vec3 axis)
	{
		Interval a = findInterval(t1, axis);
		Interval b = findInterval(t2, axis);

		return ((b.min <= a.max) && (a.min <= b.max));
	}
	bool isOverlapOnAxis(const AABB& aabb, const Triangle& triangle, const glm::vec3& axis)
	{
		Interval a = findInterval(aabb, axis);
		Interval b = findInterval(triangle, axis);

		return ((b.min <= a.max) && (a.min <= b.max));
	}
	bool isOverlapOnAxis(const AABB& aabb, const OBB& obb, const glm::vec3& axis)
	{
		Interval a = findInterval(aabb, axis);
		Interval b = findInterval(obb, axis);

		return ((b.min <= a.max) && (a.min <= b.max));
	}
	bool isOverlapOnAxis(const OBB& obb, const Triangle& triangle, const glm::vec3& axis)
	{
		Interval a = findInterval(obb, axis);
		Interval b = findInterval(triangle, axis);

		return ((b.min <= a.max) && (a.min <= b.max));
	}
	bool isOverlapOnAxis(const OBB& o1, const OBB& o2, const glm::vec3& axis)
	{
		Interval a = findInterval(o1, axis);
		Interval b = findInterval(o2, axis);

		return ((b.min <= a.max) && (a.min <= b.max));
	}
#pragma endregion



#pragma region Triangle
	bool isTriangleTriangleCollision(const Triangle& t1, const Triangle& t2)
	{
		glm::vec3 t1e1 = t1.p2 - t1.p1;
		glm::vec3 t1e2 = t1.p3 - t1.p2;
		glm::vec3 t1e3 = t1.p1 - t1.p3;

		glm::vec3 t2e1 = t2.p2 - t2.p1;
		glm::vec3 t2e2 = t2.p3 - t2.p2;
		glm::vec3 t2e3 = t2.p1 - t2.p3;
	
		glm::vec3 axis[11] = {
			glm::cross(t1e1, t1e2),
			glm::cross(t2e1, t2e2),
			glm::cross(t2e1, t1e1), glm::cross(t2e1, t1e2), glm::cross(t2e1, t1e3),
			glm::cross(t2e2, t1e1), glm::cross(t2e2, t1e2), glm::cross(t2e2, t1e3),
			glm::cross(t2e3, t1e1), glm::cross(t2e3, t1e2), glm::cross(t2e3, t1e3)
		};

		for (int32 i = 0; i < 11; ++i)
		{
			if (isOverlapOnAxis(t1, t2, axis[i]) == false)
				return false;
		}
		return true;
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
		Point closest = findClosestPoint(sphere.position, triangle);
		
		float squareLength = calculateSquareLength(Line(closest, sphere.position));
		return (squareLength <= sphere.radius * sphere.radius);
	}

	bool isTriangleAABBCollision(const Triangle& triangle, const AABB& aabb)
	{
		glm::vec3 e1 = triangle.p2 - triangle.p1;
		glm::vec3 e2 = triangle.p3 - triangle.p2;
		glm::vec3 e3 = triangle.p1 - triangle.p3;

		glm::vec3 u = glm::vec3(1.0f, 0.0f, 0.0f);
		glm::vec3 v = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::vec3 w = glm::vec3(0.0f, 0.0f, 1.0f);

		glm::vec3 axis[13] = {
			u, v, w,
			glm::cross(e1, e2),
			glm::cross(u, e1), glm::cross(u, e2), glm::cross(u, e3),
			glm::cross(v, e1), glm::cross(v, e2), glm::cross(v, e3),
			glm::cross(w, e1), glm::cross(w, e2), glm::cross(w, e3)
		};

		for (int32 i = 0; i < 13; ++i)
		{
			if (isOverlapOnAxis(aabb, triangle, axis[i]) == false)
				return false;
		}
		return true;
	}

	bool isTriangleOBBCollision(const Triangle& triangle, const OBB& obb)
	{
		glm::vec3 e1 = triangle.p2 - triangle.p1;
		glm::vec3 e2 = triangle.p3 - triangle.p2;
		glm::vec3 e3 = triangle.p1 - triangle.p3;

		glm::mat3 rotation = glm::mat3(obb.orientation);
		glm::vec3 u = glm::vec3(rotation[0][0], rotation[0][1], rotation[0][2]);
		glm::vec3 v = glm::vec3(rotation[1][0], rotation[1][1], rotation[1][2]);
		glm::vec3 w = glm::vec3(rotation[2][0], rotation[2][1], rotation[2][2]);

		glm::vec3 axis[13] = {
			u, v, w,
			glm::cross(e1, e2),
			glm::cross(u, e1), glm::cross(u, e2), glm::cross(u, e3),
			glm::cross(v, e1), glm::cross(v, e2), glm::cross(v, e3),
			glm::cross(w, e1), glm::cross(w, e2), glm::cross(w, e3)
		};

		for (int32 i = 0; i < 13; ++i)
		{
			if (isOverlapOnAxis(obb, triangle, axis[i]) == false)
				return false;
		}
		return true;
	}

	/*
	bool isTriangleModelCollision(const Triangle& triangle, const Model& model)
	{
		glm::mat4 worldMatrix = model.getWorldMatrix();
		glm::mat4 invWorld = glm::inverse(worldMatrix);

		Triangle localTriangle;
		localTriangle.p1 = glm::vec3(invWorld * glm::vec4(triangle.p1, 1.0f));
		localTriangle.p2 = glm::vec3(invWorld * glm::vec4(triangle.p2, 1.0f));
		localTriangle.p3 = glm::vec3(invWorld * glm::vec4(triangle.p3, 1.0f));

		if (model.getMesh() != nullptr)
			return isMeshTriangleCollision(*(model.getMesh()), localTriangle);
		return false;
	}
	*/
#pragma endregion



#pragma region Plane
	float planeEquation(const Point& point, const Plane& plane)
	{
		// 0 : on plane
		// positive : front of plane
		// negative : behind of plane
		return (glm::dot(point, plane.normal) - plane.distance);
	}

	Plane makePlaneFromTriangle(const Triangle& triangle)
	{
		Plane result;

		result.normal = glm::normalize(glm::cross(triangle.p2 - triangle.p1, triangle.p3 - triangle.p1));
		result.distance = glm::dot(result.normal, triangle.p1);
		return result;
	}

	bool isPlaneTriangleCollision(const Plane& plane, const Triangle& triangle)
	{
		return isTrianglePlaneCollision(triangle, plane);
	}

	bool isPlanePlaneCollision(const Plane& p1, const Plane& p2)
	{
		float similarity = glm::dot(p1.normal, p2.normal);

		return (fabsf(similarity - 1.0f) > EPSILON);
	}

	bool isPlaneSphereCollision(const Plane& plane, const Sphere& sphere)
	{
		return isSpherePlaneCollision(sphere, plane);
	}

	bool isPlaneAABBCollision(const Plane& plane, const AABB& aabb)
	{
		return isAABBPlaneCollision(aabb, plane);
	}

	bool isPlaneOBBCollision(const Plane& plane, const OBB& obb)
	{
		return isOBBPlaneCollision(obb, plane);
	}

	/*
	bool isPlaneModelCollision(const Plane& plane, const Model& model)
	{
		glm::mat4 worldMatrix = model.getWorldMatrix();
		glm::mat4 invWorld = glm::inverse(worldMatrix);

		Plane localPlane;
		localPlane.normal = glm::vec3(invWorld * glm::vec4(plane.normal, 0.0f));
		//Recheck Here
		localPlane.distance = plane.distance;

		if (model.getMesh() != nullptr)
			return isMeshPlaneCollision(*(model.getMesh()), localPlane);
		return false;
	}
	*/
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

	Ray makeRayFromPoints(const Point& from, const Point& to)
	{
		glm::vec3 direction = glm::normalize(to - from);
		return Ray(from, direction);
	}

	glm::vec3 findBarycentricCoordinate(const Point& point, const Triangle& triangle)
	{
		glm::vec3 ap = point - triangle.p1;
		glm::vec3 bp = point - triangle.p2;
		glm::vec3 cp = point - triangle.p3;

		glm::vec3 ab = triangle.p2 - triangle.p1;
		glm::vec3 ac = triangle.p3 - triangle.p1;
		glm::vec3 bc = triangle.p3 - triangle.p2;
		glm::vec3 cb = triangle.p2 - triangle.p3;
		glm::vec3 ca = triangle.p1 - triangle.p3;

		glm::vec3 perp = ab - glm::proj(ab, cb);
		float a = 1.0f - (glm::dot(perp, ap) / glm::dot(perp, ab));

		perp = bc - glm::proj(bc, ac);
		float b = 1.0f - (glm::dot(perp, bp) / glm::dot(perp, bc));

		perp = ca - glm::proj(ca, ab);
		float c = 1.0f - (glm::dot(perp, cp) / glm::dot(perp, ca));

		return glm::vec3(a, b, c);
	}

	void raycast(const Ray& ray, const Triangle& triangle, RaycastInfo* outInfo)
	{
		resetRaycastInfo(outInfo);

		Plane plane = makePlaneFromTriangle(triangle);
		raycast(ray, plane, outInfo);
		
		if (outInfo->isHit == false)
			return;

		glm::vec3 barycentric = findBarycentricCoordinate(outInfo->hitPoint, triangle);
		if (barycentric.x >= 0.0f && barycentric.x <= 1.0f &&
			barycentric.y >= 0.0f && barycentric.y <= 1.0f &&
			barycentric.z >= 0.0f && barycentric.z <= 1.0f)
			return;

		resetRaycastInfo(outInfo);
	}
	void raycast(const Ray& ray, const Plane& plane, RaycastInfo* outInfo)
	{
		resetRaycastInfo(outInfo);

		float nd = glm::dot(ray.direction, plane.normal);
		float pn = glm::dot(ray.origin, plane.normal);

		if (nd >= 0.0f)
			return;

		float t = (plane.distance - pn) / nd;

		if (t >= 0.0f && outInfo != nullptr)
		{
			outInfo->hitPoint = ray.origin + t * ray.direction;
			outInfo->normal = glm::normalize(plane.normal);
			outInfo->rayTime = t;
			outInfo->isHit = true;
		}
	}
	void raycast(const Ray& ray, const Sphere& sphere, RaycastInfo* outInfo)
	{
		resetRaycastInfo(outInfo);

		glm::vec3 toSphere = sphere.position - ray.origin;

		float squareRadius = sphere.radius * sphere.radius;
		float squareDistance = glm::dot(toSphere, toSphere);
		float tangentLength = glm::dot(toSphere, ray.direction);
		
		if (squareRadius + tangentLength * tangentLength < squareDistance)
			return;

		float squarePerpLength = squareDistance - (tangentLength * tangentLength);
		float penetratedLength = glm::sqrt(squareRadius - squarePerpLength);

		// Outer || Inner Check
		float t = (squareDistance > squareRadius) ?  tangentLength - penetratedLength : tangentLength + penetratedLength;

		if (outInfo != nullptr)
		{
			outInfo->hitPoint = ray.origin + t * ray.direction;
			outInfo->normal = glm::normalize(outInfo->hitPoint - sphere.position);
			outInfo->rayTime = t;
			outInfo->isHit = true;
		}
	}
	void raycast(const Ray& ray, const AABB& aabb, RaycastInfo* outInfo)
	{
		resetRaycastInfo(outInfo);

		glm::vec3 min = getMinFromAABB(aabb);
		glm::vec3 max = getMaxFromAABB(aabb);

		float t1 = (min.x - ray.origin.x) / ((glm::abs(ray.direction.x) < EPSILON) ? EPSILON : ray.direction.x);
		float t2 = (max.x - ray.origin.x) / ((glm::abs(ray.direction.x) < EPSILON) ? EPSILON : ray.direction.x);
		float t3 = (min.y - ray.origin.y) / ((glm::abs(ray.direction.y) < EPSILON) ? EPSILON : ray.direction.y);
		float t4 = (max.y - ray.origin.y) / ((glm::abs(ray.direction.y) < EPSILON) ? EPSILON : ray.direction.y);
		float t5 = (min.z - ray.origin.z) / ((glm::abs(ray.direction.z) < EPSILON) ? EPSILON : ray.direction.z);
		float t6 = (max.z - ray.origin.z) / ((glm::abs(ray.direction.z) < EPSILON) ? EPSILON : ray.direction.z);

		float tmin = fmaxf(fmaxf(fminf(t1, t2), fminf(t3, t4)), fminf(t5, t6));
		float tmax = fminf(fminf(fmaxf(t1, t2), fmaxf(t3, t4)), fmaxf(t5, t6));

		if (tmax < 0.0f)
			return;

		if (tmin > tmax)
			return;

		float tresult = tmin;

		if (tmin < 0.0f)
			tresult = tmax;

		if (outInfo != nullptr)
		{
			outInfo->isHit = true;
			outInfo->rayTime = tresult;
			outInfo->hitPoint = ray.origin + tresult * ray.direction;

			glm::vec3 normals[6] = {
				glm::vec3(-1.0f, 0.0f, 0.0f),
				glm::vec3(1.0f, 0.0f, 0.0f),
				glm::vec3(0.0f, -1.0f, 0.0f),
				glm::vec3(0.0f, 1.0f, 0.0f),
				glm::vec3(0.0f, 0.0f, -1.0f),
				glm::vec3(0.0f, 0.0f, 1.0f)
			};
			float t[6] = { t1, t2, t3, t4, t5, t6 };

			for (int32 i = 0; i < 6; ++i)
			{
				if (glm::abs(tresult - t[i]) < EPSILON)
					outInfo->normal = normals[i];
			}
		}
	}
	void raycast(const Ray& ray, const OBB& obb, RaycastInfo* outInfo)
	{
		resetRaycastInfo(outInfo);

		glm::vec3 halfSide = obb.size;
		glm::mat3 rotation = glm::mat3(obb.orientation);
		glm::vec3 basis[3] = {
			glm::vec3(rotation[0][0], rotation[0][1], rotation[0][2]),
			glm::vec3(rotation[1][0], rotation[1][1], rotation[1][2]),
			glm::vec3(rotation[2][0], rotation[2][1], rotation[2][2])
		};

		glm::vec3 rayToOBB = obb.position - ray.origin;
		glm::vec3 f = glm::vec3(glm::dot(basis[0], ray.direction), glm::dot(basis[1], ray.direction), glm::dot(basis[2], ray.direction));
		glm::vec3 e = glm::vec3(glm::dot(basis[0], rayToOBB), glm::dot(basis[1], rayToOBB), glm::dot(basis[2], rayToOBB));

		float t[6] = { 0, 0, 0, 0, 0, 0 };
		for (int32 i = 0; i < 3; ++i)
		{
			if (glm::abs(f[i]) < EPSILON)
			{
				if (-e[i] - halfSide[i] > 0.0f || -e[i] + halfSide[i] < 0.0f)
					return;

				f[i] = 0.00001f;
			}

			t[i * 2] = (e[i] + halfSide[i]) / f[i];
			t[i * 2 + 1] = (e[i] - halfSide[i]) / f[i];
		}

		float tmin = fmaxf(fmaxf(fminf(t[0], t[1]), fminf(t[2], t[3])), fminf(t[4], t[5]));
		float tmax = fminf(fminf(fmaxf(t[0], t[1]), fmaxf(t[2], t[3])), fmaxf(t[4], t[5]));

		if (tmax < 0.0f)
			return;

		if (tmin > tmax)
			return;

		float tresult = tmin;
		if (tmin < 0.0f)
			tresult = tmax;

		if (outInfo != nullptr)
		{
			outInfo->isHit = true;
			outInfo->rayTime = tresult;
			outInfo->hitPoint = ray.origin + tresult * ray.direction;

			glm::vec3 normals[6] = {
				basis[0], -basis[0], basis[1], -basis[1], basis[2], -basis[2]
			};

			for (int32 i = 0; i < 6; ++i)
			{
				if (glm::abs(tresult - t[i]) < EPSILON)
					outInfo->normal = glm::normalize(normals[i]);
			}
		}
	}

	/*
	//Recheck Here
	void raycast(const Ray& ray, const Mesh& mesh, RaycastInfo* outInfo)
	{
		resetRaycastInfo(outInfo);
		int32 n = mesh.numTriangles;

		for (int32 i = 0; i < n; ++i)
		{
			raycast(ray, mesh.triangles[i], outInfo);
			if (outInfo->isHit)
				return;
		}
	}
	void raycast(const Ray& ray, const Model& model, RaycastInfo* outInfo)
	{
		resetRaycastInfo(outInfo);

		__noop;
	}
	*/
#pragma endregion



#pragma region Sphere
	bool isSphereTriangleCollision(const Sphere& sphere, const Triangle& triangle)
	{
		return isTriangleSphereCollision(triangle, sphere);
	}

	bool isSpherePlaneCollision(const Sphere& sphere, const Plane& plane)
	{
		Point closestPoint = findClosestPoint(sphere.position, plane);

		float squareLength = calculateSquareLength(Line(sphere.position, closestPoint));
		return (squareLength < sphere.radius * sphere.radius);
	}

	bool isSphereSphereCollision(const Sphere& s1, const Sphere& s2)
	{
		float squareLength = calculateSquareLength(Line(s1.position, s2.position));
		float radiusSum = s1.radius + s2.radius;

		return (squareLength < radiusSum * radiusSum);
	}

	bool isSphereAABBCollision(const Sphere& sphere, const AABB& aabb)
	{
		Point closestPoint = findClosestPoint(sphere.position, aabb);

		float squareLength = calculateSquareLength(Line(sphere.position, closestPoint));
		return (squareLength < sphere.radius * sphere.radius);
	}

	bool isSphereOBBCollision(const Sphere& sphere, const OBB& obb)
	{
		Point closestPoint = findClosestPoint(sphere.position, obb);

		float squareLength = calculateSquareLength(Line(sphere.position, closestPoint));
		return (squareLength < sphere.radius * sphere.radius);
	}

	/*
	bool isSphereModelCollision(const Sphere& sphere, const Model& model)
	{
		glm::mat4 worldMatrix = model.getWorldMatrix();
		glm::mat4 invWorld = glm::inverse(worldMatrix);

		Sphere localSphere;
		localSphere.position = glm::vec3(invWorld * glm::vec4(sphere.position, 1.0f));
		localSphere.radius = sphere.radius;

		if (model.getMesh() != nullptr)
			return isMeshSphereCollision(*(model.getMesh()), sphere);
		return false;
	}
	*/
#pragma endregion



#pragma region AABB
	Point getMinFromAABB(const AABB& aabb)
	{
		Point p1 = aabb.position + aabb.size;
		Point p2 = aabb.position - aabb.size;

		return Point(fminf(p1.x, p2.x), fminf(p1.y, p2.y), fminf(p1.z, p2.z));
	}

	Point getMaxFromAABB(const AABB& aabb)
	{
		Point p1 = aabb.position + aabb.size;
		Point p2 = aabb.position - aabb.size;

		return Point(fmaxf(p1.x, p2.x), fmaxf(p1.y, p2.y), fmaxf(p1.z, p2.z));
	}

	AABB makeAABBFromMinMax(const Point& min, const Point& max)
	{
		glm::vec3 position = (min + max) * 0.5f;
		glm::vec3 size = (max - min) * 0.5f;

		return AABB(position, size);
	}

	bool isAABBTriangleCollision(const AABB& aabb, const Triangle& triangle)
	{
		return isTriangleAABBCollision(triangle, aabb);
	}

	bool isAABBPlaneCollision(const AABB& aabb, const Plane& plane)
	{
		float projection = aabb.size.x * fabsf(plane.normal.x)
			+ aabb.size.y * fabsf(plane.normal.y)
			+ aabb.size.z * fabsf(plane.normal.z);

		float dot = glm::dot(plane.normal, aabb.position);
		float dist = dot - plane.distance;

		return (fabsf(dist) <= projection);
	}

	bool isAABBSphereCollision(const AABB& aabb, const Sphere& sphere)
	{
		return isSphereAABBCollision(sphere, aabb);
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

	bool isAABBOBBCollision(const AABB& aabb, const OBB& obb)
	{
		glm::mat3 rotation = glm::mat3(obb.orientation);

		glm::vec3 axis[15] = {
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(0.0f, 0.0f, 1.0f),
			glm::vec3(rotation[0][0], rotation[0][1], rotation[0][2]),
			glm::vec3(rotation[1][0], rotation[1][1], rotation[1][2]),
			glm::vec3(rotation[2][0], rotation[2][1], rotation[2][2])
		};

		for (int32 i = 0; i < 3; ++i)
		{
			axis[6 + i * 3] = glm::cross(axis[i], axis[0]);
			axis[6 + i * 3 + 1] = glm::cross(axis[i], axis[1]);
			axis[6 + i * 3 + 2] = glm::cross(axis[i], axis[2]);
		}

		for (int32 i = 0; i < 15; ++i)
		{
			if (isOverlapOnAxis(aabb, obb, axis[i]) == false)
				return false;
		}
		return true;
	}
	/*
	bool isAABBModelCollision(const AABB& aabb, const Model& model)
	{
		glm::mat4 worldMatrix = model.getWorldMatrix();
		glm::mat4 invWorld = glm::inverse(worldMatrix);

		OBB localOBB;
		localOBB.size = aabb.size;
		localOBB.position = glm::vec3(invWorld * glm::vec4(aabb.position, 1.0f));
		localOBB.orientation = glm::quat_cast(glm::mat3(invWorld));

		if (model.getMesh() != nullptr)
			return isMeshOBBCollision(*(model.getMesh()), localOBB);
		return false;
	}
	*/
#pragma endregion



#pragma region OBB
	void renderOBB(const OBB& obb, Shader& shader)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glBindVertexArray(obb.vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obb.ebo);

		glm::mat4 worldMat = glm::translate(glm::mat4(1.0f), obb.position) * glm::mat4_cast(obb.orientation);
		shader.setUniformMat4("worldMat", worldMat);
		glLineWidth(2.0f);
		glDrawElements(GL_LINES, 36, GL_UNSIGNED_INT, NULL);
	}

	bool isOBBTriangleCollision(const OBB& obb, const Triangle& triangle)
	{
		return isTriangleOBBCollision(triangle, obb);
	}

	bool isOBBPlaneCollision(const OBB& obb, const Plane& plane)
	{
		glm::mat3 rotation = glm::mat3(obb.orientation);
		glm::vec3 basis[3] = {
			glm::vec3(rotation[0][0], rotation[0][1], rotation[0][2]),
			glm::vec3(rotation[1][0], rotation[1][1], rotation[1][2]),
			glm::vec3(rotation[2][0], rotation[2][1], rotation[2][2])
		};

		float projection = obb.size.x * fabsf(glm::dot(plane.normal, basis[0]))
			+ obb.size.y * fabsf(glm::dot(plane.normal, basis[1]))
			+ obb.size.z * fabsf(glm::dot(plane.normal, basis[2]));

		float dot = glm::dot(plane.normal, obb.position);
		float dist = dot - plane.distance;

		return (fabsf(dist) <= projection);
	}

	bool isOBBSphereCollision(const OBB& obb, const Sphere& sphere)
	{
		return isSphereOBBCollision(sphere, obb);
	}

	bool isOBBAABBCollision(const OBB& obb, const AABB& aabb)
	{
		return isAABBOBBCollision(aabb, obb);
	}

	bool isOBBOBBCollision(const OBB& o1, const OBB& o2)
	{
		glm::mat3 rotation1 = glm::mat3(o1.orientation);
		glm::mat3 rotation2 = glm::mat3(o2.orientation);

		glm::vec3 axis[15] = {
			glm::vec3(rotation1[0][0], rotation1[0][1], rotation1[0][2]),
			glm::vec3(rotation1[1][0], rotation1[1][1], rotation1[1][2]),
			glm::vec3(rotation1[2][0], rotation1[2][1], rotation1[2][2]),
			glm::vec3(rotation2[0][0], rotation2[0][1], rotation2[0][2]),
			glm::vec3(rotation2[1][0], rotation2[1][1], rotation2[1][2]),
			glm::vec3(rotation2[2][0], rotation2[2][1], rotation2[2][2])
		};

		for (int32 i = 0; i < 3; ++i)
		{
			axis[6 + i * 3] = glm::cross(axis[i], axis[0]);
			axis[6 + i * 3 + 1] = glm::cross(axis[i], axis[1]);
			axis[6 + i * 3 + 2] = glm::cross(axis[i], axis[2]);
		}

		for (int32 i = 0; i < 15; ++i)
		{
			if (isOverlapOnAxis(o1, o2, axis[i]) == false)
				return false;
		}
		return true;
	}

	/*
	bool isOBBModelCollision(const OBB& obb, const Model& model)
	{
		glm::mat4 worldMatrix = model.getWorldMatrix();
		glm::mat4 invWorld = glm::inverse(worldMatrix);

		OBB localOBB;
		localOBB.size = obb.size;
		localOBB.position = glm::vec3(invWorld * glm::vec4(obb.position, 1.0f));
		//Recheck Here
		localOBB.orientation = obb.orientation * glm::quat_cast(glm::mat3(invWorld));

		if (model.getMesh() != nullptr)
			return isMeshOBBCollision(*(model.getMesh()), localOBB);
		return false;
	}
	*/
#pragma endregion


/*
#pragma region Mesh
	bool isMeshTriangleCollision(const Mesh& mesh, const Triangle& triangle)
	{
		int32 n = mesh.numTriangles;

		for (int32 i = 0; i < n; ++i)
		{
			if (isTriangleTriangleCollision(mesh.triangles[i], triangle) == true)
				return true;
		}
		return false;
	}

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
#pragma endregion



//#pragma region Frustum
//	Point findIntersectionPoint(const Plane& p1, const Plane& p2, const Plane& p3)
//	{
//		__noop;
//	}
//
//	void getCorners(const Frustum& f, glm::vec3* outCorners)
//	{
//		__noop;
//	}
//#pragma endregion



#pragma region Model
	void Model::setMesh(Mesh* mesh)
	{
		_mesh = mesh;

		if (_mesh != nullptr)
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
			_boundary = makeAABBFromMinMax(min, max);
		}
	}

	OBB Model::getOBB(void)
	{
		OBB obb;

		glm::mat4 worldMatrix = getWorldMatrix();
		AABB boundary = getBoundary();
		
		//Recheck Here
		obb.size = boundary.size;
		obb.position = glm::vec3(worldMatrix * glm::vec4(boundary.position, 1.0f));
		obb.orientation = glm::quat(glm::mat3(worldMatrix));
		
		return obb;
	}

	glm::mat4 Model::getWorldMatrix(void) const
	{
		glm::mat4 translation = glm::translate(glm::mat4(1.0f), _position);
		glm::mat4 rotation = glm::mat4_cast(_rotation);

		//Recheck Here
		glm::mat4 localMatrix = rotation * translation;

		if (_parent != nullptr)
		{
			glm::mat4 parentMatrix = _parent->getWorldMatrix();
			return parentMatrix * localMatrix;
		}
		return localMatrix;
	}
#pragma endregion
*/


#pragma region Collision
	void resetCollisionManifold(CollisionManifold* manifold)
	{
		if (manifold != nullptr)
		{
			manifold->isColliding = false;
			manifold->normal = glm::vec3(0.0f, 0.0f, 1.0f);
			manifold->depth = 0.0f;
			manifold->contacts.clear();
		}
	}

	void findCollisionManifold(const Sphere& s1, const Sphere& s2, CollisionManifold* outManifold)
	{
		resetCollisionManifold(outManifold);

		float radiusSum = s1.radius + s2.radius;
		float squareDistance = calculateSquareLength(Line(s1.position, s2.position));

		if (squareDistance > radiusSum * radiusSum || squareDistance <= EPSILON)
			return;

		if (outManifold != nullptr)
		{
			outManifold->isColliding = true;
			outManifold->normal = glm::normalize(s2.position - s1.position);
			outManifold->depth = glm::abs(glm::sqrt(squareDistance) - radiusSum) * 0.5f;
			
			float distance = s1.radius - outManifold->depth;
			Point contact = s1.position + outManifold->normal * distance;
			outManifold->contacts.push_back(contact);
		}
	}
	void findCollisionManifold(const OBB& obb, const Sphere& sphere, CollisionManifold* outManifold)
	{
		resetCollisionManifold(outManifold);

		Point closest = findClosestPoint(sphere.position, obb);

		float squareDistance = calculateSquareLength(Line(closest, sphere.position));
		if (squareDistance > sphere.radius * sphere.radius)
			return;

		glm::vec3 normal;
		if (squareDistance < EPSILON)
		{
			if (calculateSquareLength(Line(closest, obb.position)) < EPSILON)
				return;

			normal = glm::normalize(closest - obb.position);
		}
		else
			normal = glm::normalize(sphere.position - closest);

		Point outsidePoint = sphere.position - normal * sphere.radius;

		float distance = glm::length(closest - outsidePoint);

		if (outManifold != nullptr)
		{
			outManifold->isColliding = true;
			outManifold->normal = normal;
			outManifold->depth = distance * 0.5f;
			outManifold->contacts.push_back(closest + (outsidePoint - closest) * 0.5f);
		}
	}

	static std::vector<Point> getVerticesFromOBB(const OBB& obb)
	{
		std::vector<Point> vertices;
		vertices.resize(8);

		glm::vec3 center = obb.position;
		glm::vec3 halfSide = obb.size;
		glm::mat3 rotation = glm::mat3(obb.orientation);
		glm::vec3 basis[3] = {
			glm::vec3(rotation[0][0], rotation[0][1], rotation[0][2]),
			glm::vec3(rotation[1][0], rotation[1][1], rotation[1][2]),
			glm::vec3(rotation[2][0], rotation[2][1], rotation[2][2])
		};

		vertices[0] = center + basis[0] * halfSide[0] + basis[1] * halfSide[1] + basis[2] * halfSide[2];
		vertices[1] = center + basis[0] * halfSide[0] + basis[1] * halfSide[1] - basis[2] * halfSide[2];
		vertices[2] = center + basis[0] * halfSide[0] - basis[1] * halfSide[1] + basis[2] * halfSide[2];
		vertices[3] = center + basis[0] * halfSide[0] - basis[1] * halfSide[1] - basis[2] * halfSide[2];
		vertices[4] = center - basis[0] * halfSide[0] + basis[1] * halfSide[1] + basis[2] * halfSide[2];
		vertices[5] = center - basis[0] * halfSide[0] + basis[1] * halfSide[1] - basis[2] * halfSide[2];
		vertices[6] = center - basis[0] * halfSide[0] - basis[1] * halfSide[1] + basis[2] * halfSide[2];
		vertices[7] = center - basis[0] * halfSide[0] - basis[1] * halfSide[1] - basis[2] * halfSide[2];

		return vertices;
	}
	static std::vector<Line> getEdgesFromOBB(const OBB& obb)
	{
		std::vector<Line> edges;
		edges.reserve(12);

		std::vector<Point> vertices = getVerticesFromOBB(obb);

		int32 indices[12][2] = {
			{6, 1}, {6, 3}, {6, 4}, {2, 7}, {2, 5}, {2, 0},
			{0, 1}, {0, 3}, {7, 1}, {7, 4}, {4, 5}, {5, 3}
		};

		for (int32 i = 0; i < 12; ++i)
			edges.push_back(Line(vertices[indices[i][0]], vertices[indices[i][1]]));

		return edges;
	}
	static std::vector<Plane> getFacesFromOBB(const OBB& obb)
	{
		std::vector<Plane> faces;
		faces.resize(6);

		glm::vec3 center = obb.position;
		glm::vec3 halfSide = obb.size;
		glm::mat3 rotation = glm::mat3(obb.orientation);
		glm::vec3 basis[3] = {
			glm::vec3(rotation[0][0], rotation[0][1], rotation[0][2]),
			glm::vec3(rotation[1][0], rotation[1][1], rotation[1][2]),
			glm::vec3(rotation[2][0], rotation[2][1], rotation[2][2])
		};

		faces[0] = Plane(glm::dot(basis[0], (center + basis[0] * halfSide.x)), basis[0]);
		faces[1] = Plane(-glm::dot(basis[0], (center - basis[0] * halfSide.x)), -basis[0]);
		faces[2] = Plane(glm::dot(basis[1], (center + basis[1] * halfSide.y)), basis[1]);
		faces[3] = Plane(-glm::dot(basis[1], (center - basis[1] * halfSide.y)), -basis[1]);
		faces[4] = Plane(glm::dot(basis[2], (center + basis[2] * halfSide.z)), basis[2]);
		faces[5] = Plane(-glm::dot(basis[2], (center - basis[2] * halfSide.z)), -basis[2]);

		return faces;
	}
	static bool clipToPlane(const Plane& plane, const Line& line, Point* outPoint)
	{
		glm::vec3 direction = line.p2 - line.p1;

		float nA = glm::dot(plane.normal, line.p1);
		float nAB = glm::dot(plane.normal, direction);

		if (nAB < EPSILON)
			return false;

		float t = (plane.distance - nA) / nAB;
		if (t >= 0.0f && t <= 1.0f)
		{
			if (outPoint != nullptr)
				*outPoint = line.p1 + t * direction;
			return true;
		}

		return false;
	}
	static std::vector<Point> clipEdgesToOBB(const std::vector<Line>& edges, const OBB& obb)
	{
		std::vector<Point> points;
		points.reserve(edges.size() * 3);

		std::vector<Plane> faces = getFacesFromOBB(obb);

		Point intersection;
		int32 numEdges = edges.size();

		for (int32 i = 0; i < 6; ++i)
		{
			for (int32 j = 0; j < numEdges; ++j)
			{
				if (clipToPlane(faces[i], edges[j], &intersection) == true)
				{
					if (isPointInside(intersection, obb) == true)
						points.push_back(intersection);
				}
			}
		}

		return points;
	}
	static float penetrationDepth(const OBB& o1, const OBB& o2, const glm::vec3& axis, bool* outShouldFlip)
	{
		Interval interval1 = findInterval(o1, glm::normalize(axis));
		Interval interval2 = findInterval(o2, glm::normalize(axis));

		if (((interval2.min <= interval1.max) && (interval1.min <= interval2.max)) == false)
			return 0.0f;

		float len1 = interval1.max - interval1.min;
		float len2 = interval2.max - interval2.min;
		float min = fminf(interval1.min, interval2.min);
		float max = fmaxf(interval1.max, interval2.max);
		float length = max - min;

		if (outShouldFlip != nullptr)
			*outShouldFlip = (interval2.min < interval1.min);

		return (len1 + len2) - length;
	}
	void findCollisionManifold(const OBB& o1, const OBB& o2, CollisionManifold* outManifold)
	{
		resetCollisionManifold(outManifold);

		Sphere s1(o1.position, glm::length(o1.size));
		Sphere s2(o2.position, glm::length(o2.size));

		if (isSphereSphereCollision(s1, s2) == false)
			return;

		glm::mat3 rotation1 = glm::mat3(o1.orientation);
		glm::vec3 basis1[3] = {
			glm::vec3(rotation1[0][0], rotation1[0][1], rotation1[0][2]),
			glm::vec3(rotation1[1][0], rotation1[1][1], rotation1[1][2]),
			glm::vec3(rotation1[2][0], rotation1[2][1], rotation1[2][2])
		};

		glm::mat3 rotation2 = glm::mat3(o2.orientation);
		glm::vec3 basis2[3] = {
			glm::vec3(rotation2[0][0], rotation2[0][1], rotation2[0][2]),
			glm::vec3(rotation2[1][0], rotation2[1][1], rotation2[1][2]),
			glm::vec3(rotation2[2][0], rotation2[2][1], rotation2[2][2])
		};

		glm::vec3 axis[15] = {
			basis1[0], basis1[1], basis1[2],
			basis2[0], basis2[1], basis2[2]
		};

		for (int32 i = 0; i < 3; ++i)
		{
			axis[6 + i * 3] = glm::cross(axis[i], axis[0]);
			axis[6 + i * 3 + 1] = glm::cross(axis[i], axis[1]);
			axis[6 + i * 3 + 2] = glm::cross(axis[i], axis[2]);
		}

		glm::vec3* hitNormal = nullptr;
		bool shouldFlip;

		for (int32 i = 0; i < 15; ++i)
		{
			if (axis[i].x < 0.000001f) axis[i].x = 0.0f;
			if (axis[i].y < 0.000001f) axis[i].y = 0.0f;
			if (axis[i].z < 0.000001f) axis[i].z = 0.0f;

			if (glm::dot(axis[i], axis[i]) < EPSILON)
				continue;

			float depth = penetrationDepth(o1, o2, axis[i], &shouldFlip);
			if (depth <= 0.0f)
				return;
			else if (depth < outManifold->depth)
			{
				if (shouldFlip == true)
					axis[i] = -axis[i];

				outManifold->depth = depth;
				hitNormal = &axis[i];
			}
		}

		if (hitNormal == nullptr)
			return;

		glm::vec3 normal = glm::normalize(*hitNormal);

		std::vector<Point> c1 = clipEdgesToOBB(getEdgesFromOBB(o2), o1);
		std::vector<Point> c2 = clipEdgesToOBB(getEdgesFromOBB(o1), o2);

		outManifold->contacts.reserve(c1.size() + c2.size());
		outManifold->contacts.insert(outManifold->contacts.end(), c1.begin(), c1.end());
		outManifold->contacts.insert(outManifold->contacts.end(), c2.begin(), c2.end());

		Interval interval = findInterval(o1, normal);
		float distance = (interval.max - interval.min) * 0.5f - outManifold->depth * 0.5f;
		Point pointOnPlane = o1.position + normal * distance;

		for (int32 i = outManifold->contacts.size() - 1; i >= 0; --i)
		{
			Point contact = outManifold->contacts[i];
			outManifold->contacts[i] = contact + (normal * glm::dot(normal, pointOnPlane - contact));

			for (int32 j = outManifold->contacts.size() - 1; j > i; --j)
			{
				if (calculateSquareLength(Line(outManifold->contacts[i], outManifold->contacts[j])) < 0.0001f)
				{
					outManifold->contacts.erase(outManifold->contacts.begin() + j);
					break;
				}
			}
		}

		outManifold->isColliding = true;
		outManifold->normal = normal;
	}
#pragma endregion

}