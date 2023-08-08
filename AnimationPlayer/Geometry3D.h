#pragma once
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "CommonTypes.h"

namespace pa
{
	struct Line;
	struct Triangle;
	struct Plane;
	struct Sphere;
	struct AABB;
	struct OBB;
	struct Cylinder;
	struct Mesh;

	struct Frustum;
}

namespace pa
{
	typedef glm::vec3 Point;

	bool isPointInside(const Point& point, const Line& line);
	bool isPointInside(const Point& point, const Triangle& triangle);
	bool isPointInside(const Point& point, const Plane& plane);
	bool isPointInside(const Point& point, const Sphere& sphere);
	bool isPointInside(const Point& point, const AABB& aabb);
	bool isPointInside(const Point& point, const OBB& obb);
	bool isPointInside(const Point& point, const Cylinder& cylinder);

	Point getClosestPoint(const Point& point, const Line& line);
	Point getClosestPoint(const Point& point, const Triangle& triangle);
	Point getClosestPoint(const Point& point, const Plane& plane);
	Point getClosestPoint(const Point& point, const Sphere& sphere);
	Point getClosestPoint(const Point& point, const AABB& aabb);
	Point getClosestPoint(const Point& point, const OBB& obb);
	Point getClosestPoint(const Point& point, const Cylinder& cylinder);
}

namespace pa
{
	struct Line
	{
		Line(void) { }
		Line(const Point& pt1, const Point& pt2) : p1(pt1), p2(pt2) { }

		Point p1;
		Point p2;
	};

	float getLineLength(const Line& line);
}

namespace pa
{
	struct Triangle
	{
		Triangle(void) { }
		Triangle(const Point& pt1, const Point& pt2, const Point& pt3) : p1(pt1), p2(pt2), p3(pt3) { }

		union
		{
			struct
			{
				Point p1;
				Point p2;
				Point p3;
			};

			Point	points[3];
			float	values[9];
		};
	};

	bool isTriangleTriangleCollision(const Triangle& t1, const Triangle& t2);
	bool isTrianglePlaneCollision(const Triangle& triangle, const Plane& plane);
	bool isTriangleSphereCollision(const Triangle& triangle, const Sphere& sphere);
	bool isTriangleAABBCollision(const Triangle& triangle, const AABB& aabb);
	bool isTriangleOBBCollision(const Triangle& triangle, const OBB& obb);
	bool isTriangleCylinderCollision(const Triangle& triangle, const Cylinder& cylinder);
}

namespace pa
{
	struct Plane
	{
		Plane(void) { }
		Plane(const float d, const glm::vec3& n) : distance(d), normal(n) { }

		float		distance;
		glm::vec3	normal;
	};

	float planeEquation(const Point& point, const Plane& plane);
	Plane makePlaneFromTriangle(const Triangle& triangle);
}

namespace pa
{
	struct RaycastInfo
	{
		Point		hitPoint;
		glm::vec3	normal;
		float		rayTime;
		bool		isHit;
	};
	void resetRaycastInfo(RaycastInfo* outInfo);

	struct Ray
	{
		Ray(void) : origin(0.0f, 0.0f, 0.0f), direction(0.0f, 0.0f, 1.0f) { }
		Ray(const glm::vec3& o, const glm::vec3& d) : origin(o), direction(glm::normalize(d)) { }

		glm::vec3 origin;
		glm::vec3 direction;
	};

	glm::vec3 getBarycentricCoordinate(const Point& point, const Triangle& triangle);
	float raycast(const Ray& ray, const Triangle& triangle);
	float raycast(const Ray& ray, const Plane& plane);
	bool raycast(const Ray& ray, const Sphere& sphere, RaycastInfo* outInfo);
	bool raycast(const Ray& ray, const AABB& aabb, RaycastInfo* outInfo);
	bool raycast(const Ray& ray, const OBB& obb, RaycastInfo* outInfo);
	float raycast(const Ray& ray, const Mesh& mesh);
}

namespace pa
{
	struct AABB
	{
		AABB(void) : position(0.0f, 0.0f, 0.0f), size(1.0f, 1.0f, 1.0f) { }
		AABB(const glm::vec3& p, const glm::vec3& s) : position(p), size(s) { }

		glm::vec3 position;
		glm::vec3 size;
	};

	glm::vec3 getMinFromAABB(const AABB& aabb);
	glm::vec3 getMaxFromAABB(const AABB& aabb);
	AABB makeAABBFromMinMax(const glm::vec3& min, const glm::vec3& max);
	bool isAABBSphereCollision(const AABB& aabb, const Sphere& sphere);
	bool isAABBAABBCollision(const AABB& a1, const AABB& a2);
	bool isAABBOBBCollision(const AABB& aabb, const OBB& obb);
}

namespace pa
{
	struct OBB
	{
		OBB(void) : position(0.0f, 0.0f, 0.0f), size(1.0f, 1.0f, 1.0f) { }
		OBB(const glm::vec3& p, const glm::vec3& s) : position(p), size(s) { }
		OBB(const glm::vec3& p, const glm::vec3& s, const glm::mat3& m) : position(p), size(s), orientation(m) { }

		glm::vec3 position;
		glm::vec3 size;
		glm::mat3 orientation;
	};

	bool isOBBOBBCollision(const OBB& o1, const OBB& o2);
	bool isOBBSphereCollision(const OBB& obb, const Sphere& sphere);
	bool isOBBAABBCollision(const OBB& obb, const AABB& aabb);
}

namespace pa
{
	struct Sphere
	{
		Sphere(void) : position(0.0f, 0.0f, 0.0f), radius(1.0f) { }
		Sphere(const glm::vec3& p, float r) : position(p), radius(r) { }

		glm::vec3	position;
		float		radius;
	};

	bool isSphereSphereCollision(const Sphere& s1, const Sphere& s2);
	bool isSphereAABBCollision(const Sphere& sphere, const AABB& aabb);
	bool isSphereOBBCollision(const Sphere& sphere, const OBB& obb);
}

namespace pa
{
	struct Cylinder
	{
		Cylinder(void) : position(0.0f, 0.0f, 0.0f), direction(0.0f, 0.0f, 1.0f), radius(1.0f) { }
		Cylinder(const glm::vec3& p, const glm::vec3& d, float r) : position(p), direction(d), radius(r) { }

		glm::vec3	position;
		glm::vec3	direction;
		float		radius;
	};
}

namespace pa
{
	struct Mesh
	{
		int32 numTriangles;

		union
		{
			std::vector<Triangle>	triangles;
			std::vector<Point>		points;
			float					values;
		};
	};

	bool isMeshPlaneCollision(const Mesh& mesh, const Plane& plane);
	bool isMeshSphereCollision(const Mesh& mesh, const Sphere& sphere);
	bool isMeshAABBCollision(const Mesh& mesh, const AABB& aabb);
	bool isMeshOBBCollision(const Mesh& mesh, const OBB& obb);
	bool isMeshCylinderCollision(const Mesh& mesh, const Cylinder& cylinder);
}

namespace pa
{
	struct Frustum
	{
		Frustum() { }

		union
		{
			struct
			{
				Plane top;
				Plane bottom;
				Plane left;
				Plane right;
				Plane near;
				Plane far;
			};
			Plane planes[6];
		};
	};

	Point findIntersectionPoint(const Plane& p1, const Plane& p2, const Plane& p3);
	void getCorners(const Frustum& f, glm::vec3* outCorners);
}

namespace pa
{
	class Model
	{
	protected:
		Mesh* _mesh;
		AABB  _bounds;

	public:
		Model(void) : parent(0), _mesh(0) {}

		Mesh* getMesh(void) const { return _mesh; }
		AABB getBounds(void) const { return _bounds; }
		OBB getOBB(void);
		glm::mat4 getWorldMatrix(void) const;

		void setMesh(Mesh* mesh);

		bool isPlaneCollision(const Plane& plane);
		bool isRayCollision(const Ray& ray);
		bool isSphereCollision(const Sphere& sphere);
		bool isAABBCollision(const AABB& aabb);
		bool isOBBCollision(const OBB& obb);
		bool isCylinderCollision(const Cylinder& cylinder);

		glm::vec3 position;
		glm::vec3 rotation;
		Model* parent;
	};
}

namespace pa
{
	struct CollisionManifold
	{
		bool				isColliding;
		glm::vec3			normal;
		float				depth;
		std::vector<Point>	contacts;
	};

	void resetCollisionManifold(CollisionManifold* manifold);
	CollisionManifold findCollisionManifold(const Sphere& s1, const Sphere& s2);
	CollisionManifold findCollisionManifold(const OBB& obb, const Sphere& sphere);
	CollisionManifold findCollisionManifold(const OBB& o1, const OBB& o2);
}