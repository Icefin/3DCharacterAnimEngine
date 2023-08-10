#pragma once
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/projection.hpp>

#include "CommonTypes.h"

#include "Shader.h"

namespace pa
{
	constexpr glm::vec3 gravity = glm::vec3(0.0f, -9.81f, 0.0f);

	typedef glm::vec3 Point;

	struct Line;
	struct Interval;
	struct Triangle;
	struct Plane;
	struct Ray;
	struct Sphere;
	struct AABB;
	struct OBB;
}

namespace pa
{

	bool isPointInside(const Point& point, const Line& line);
	bool isPointInside(const Point& point, const Triangle& triangle);
	bool isPointInside(const Point& point, const Plane& plane);
	bool isPointInside(const Point& point, const Ray& ray);
	bool isPointInside(const Point& point, const Sphere& sphere);
	bool isPointInside(const Point& point, const AABB& aabb);
	bool isPointInside(const Point& point, const OBB& obb);

	Point findClosestPoint(const Point& point, const Line& line);
	Point findClosestPoint(const Point& point, const Triangle& triangle);
	Point findClosestPoint(const Point& point, const Plane& plane);
	Point findClosestPoint(const Point& point, const Ray& ray);
	Point findClosestPoint(const Point& point, const Sphere& sphere);
	Point findClosestPoint(const Point& point, const AABB& aabb);
	Point findClosestPoint(const Point& point, const OBB& obb);
}

namespace pa
{
	struct Line
	{
		Line(void) : p1(0.0f, 0.0f, 0.0f), p2(1.0f, 0.0f, 0.0f) { }
		Line(const Point& pt1, const Point& pt2) : p1(pt1), p2(pt2) { }

		Point p1;
		Point p2;
	};

	float	calculateSquareLength(const Line& line);

	bool isIntersection(const Line& line, const Triangle& triangle);
	bool isIntersection(const Line& line, const Plane& plane);
	bool isIntersection(const Line& line, const Sphere& sphere);
	bool isIntersection(const Line& line, const AABB& aabb);
	bool isIntersection(const Line& line, const OBB& obb);

	//bool isIntersection(const Line& line, const Mesh& mesh);
	//bool isIntersection(const Line& line, const Model& model);
}

namespace pa
{
	struct Interval
	{
		float min;
		float max;
	};

	Interval	findInterval(const Triangle& triangle, const glm::vec3& axis);
	Interval	findInterval(const AABB& aabb, const glm::vec3& axis);
	Interval	findInterval(const OBB& obb, const glm::vec3& axis);

	bool		isOverlapOnAxis(const Triangle& t1, const Triangle& t2, const glm::vec3 axis);
	bool		isOverlapOnAxis(const AABB& aabb, const Triangle& triangle, const glm::vec3& axis);
	bool		isOverlapOnAxis(const AABB& aabb, const OBB& obb, const glm::vec3& axis);
	bool		isOverlapOnAxis(const OBB& obb, const Triangle& triangle, const glm::vec3& axis);
	bool		isOverlapOnAxis(const OBB& o1, const OBB& o2, const glm::vec3& axis);
}

namespace pa
{
	struct Triangle
	{
		Triangle(void) : p1(0.0f, 0.0f, 0.0f), p2(1.0f, 0.0f, 0.0f), p3(0.0f, 0.0f, 1.0f) { }
		Triangle(const Point& pt1, const Point& pt2, const Point& pt3) : p1(pt1), p2(pt2), p3(pt3) { }

		Point p1;
		Point p2;
		Point p3;
	};

	bool isTriangleTriangleCollision(const Triangle& t1, const Triangle& t2);
	bool isTrianglePlaneCollision(const Triangle& triangle, const Plane& plane);
	bool isTriangleSphereCollision(const Triangle& triangle, const Sphere& sphere);
	bool isTriangleAABBCollision(const Triangle& triangle, const AABB& aabb);
	bool isTriangleOBBCollision(const Triangle& triangle, const OBB& obb);

	//bool isTriangleMeshCollision(const Triangle& triangle, const Mesh& mesh);
	//bool isTriangleModelCollision(const Triangle& triangle, const Model& model);
}

namespace pa
{
	struct Plane
	{
		Plane(void) : distance(0.0f), normal(1.0f, 0.0f, 0.0f) { }
		Plane(const float d, const glm::vec3& n) : distance(d), normal(glm::normalize(n)) { }

		float		distance;
		glm::vec3	normal;
	};

	float planeEquation(const Point& point, const Plane& plane);
	Plane makePlaneFromTriangle(const Triangle& triangle);

	bool isPlaneTriangleCollision(const Plane& plane, const Triangle& triangle);
	bool isPlanePlaneCollision(const Plane& p1, const Plane& p2);
	bool isPlaneSphereCollision(const Plane& plane, const Sphere& sphere);
	bool isPlaneAABBCollision(const Plane& plane, const AABB& aabb);
	bool isPlaneOBBCollision(const Plane& plane, const OBB& obb);

	//bool isPlaneMeshCollision(const Plane& plane, const Mesh& mesh);
	//bool isPlaneModelCollision(const Plane& plane, const Model& model);
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
	void resetRaycastInfo(_Out_ RaycastInfo* outInfo);

	struct Ray
	{
		Ray(void) : origin(0.0f, 0.0f, 0.0f), direction(0.0f, 0.0f, 1.0f) { }
		Ray(const glm::vec3& o, const glm::vec3& d) : origin(o), direction(glm::normalize(d)) { }

		glm::vec3 origin;
		glm::vec3 direction;
	};

	Ray makeRayFromPoints(const Point& from, const Point& to);
	glm::vec3 findBarycentricCoordinate(const Point& point, const Triangle& triangle);

	void raycast(const Ray& ray, const Triangle& triangle, _Out_ RaycastInfo* outInfo);
	void raycast(const Ray& ray, const Plane& plane, _Out_ RaycastInfo* outInfo);
	void raycast(const Ray& ray, const Sphere& sphere, _Out_ RaycastInfo* outInfo);
	void raycast(const Ray& ray, const AABB& aabb, _Out_ RaycastInfo* outInfo);
	void raycast(const Ray& ray, const OBB& obb, _Out_ RaycastInfo* outInfo);

	//void raycast(const Ray& ray, const Mesh& mesh, _Out_ RaycastInfo* outInfo);
	//void raycast(const Ray& ray, const Model& model, _Out_ RaycastInfo* outInfo);
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

	bool isSphereTriangleCollision(const Sphere& sphere, const Triangle& triangle);
	bool isSpherePlaneCollision(const Sphere& sphere, const Plane& plane);
	bool isSphereSphereCollision(const Sphere& s1, const Sphere& s2);
	bool isSphereAABBCollision(const Sphere& sphere, const AABB& aabb);
	bool isSphereOBBCollision(const Sphere& sphere, const OBB& obb);

	//bool isSphereMeshCollision(const Sphere& sphere, const Mesh& mesh);
	//bool isSphereModelCollision(const Sphere& sphere, const Model& model);
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

	Point getMinFromAABB(const AABB& aabb);
	Point getMaxFromAABB(const AABB& aabb);
	AABB makeAABBFromMinMax(const Point& min, const Point& max);

	bool isAABBTriangleCollision(const AABB& aabb, const Triangle& triangle);
	bool isAABBPlaneCollision(const AABB& aabb, const Plane& plane);
	bool isAABBSphereCollision(const AABB& aabb, const Sphere& sphere);
	bool isAABBAABBCollision(const AABB& a1, const AABB& a2);
	bool isAABBOBBCollision(const AABB& aabb, const OBB& obb);

	//bool isAABBMeshCollision(const AABB& aabb, const Mesh& mesh);
	//bool isAABBModelCollision(const AABB& aabb, const Model& model);
}

namespace pa
{
	struct OBB
	{
		OBB(void) : position(0.0f, 0.0f, 0.0f), size(1.0f, 1.0f, 1.0f), orientation(1.0f, 0.0f, 0.0f, 0.0f) { }
		OBB(const glm::vec3& p, const glm::vec3& s) : position(p), size(s), orientation(1.0f, 0.0f, 0.0f, 0.0f) { }
		OBB(const glm::vec3& p, const glm::vec3& s, const glm::quat& q) : position(p), size(s), orientation(glm::normalize(q))
		{
			float vertices[48] = {
				-s.x, -s.y, -s.z, 1.0f, 0.0f, 0.0f,
				-s.x, -s.y, s.z, 1.0f, 0.0f, 0.0f,
				s.x, -s.y, s.z, 1.0f, 0.0f, 0.0f,
				s.x, -s.y, -s.z, 1.0f, 0.0f, 0.0f,

				-s.x, s.y, -s.z, 1.0f, 0.0f, 0.0f,
				-s.x, s.y, s.z, 1.0f, 0.0f, 0.0f,
				s.x, s.y, s.z, 1.0f, 0.0f, 0.0f,
				s.x, s.y, -s.z, 1.0f, 0.0f, 0.0f
			};

			uint32 indices[36] = {
				 1,0,2,
				 2,0,3,
				 1,2,5,
				 2,6,5,
				 3,7,6,
				 3,6,2,
				 6,4,5,
				 6,7,4,
				 1,5,4,
				 1,4,0,
				 0,4,7,
				 3,0,7
			};

			glGenBuffers(1, &vbo);
			glGenVertexArrays(1, &vao);

			glBindVertexArray(vao);

			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);

			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(1);

			glGenBuffers(1, &ebo);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
		}

		glm::vec3 position;
		glm::vec3 size;
		glm::quat orientation;

		GLuint vao;
		GLuint vbo;
		GLuint ebo;
	};

	bool isOBBTriangleCollision(const OBB& obb, const Triangle& triangle);
	bool isOBBPlaneCollision(const OBB& obb, const Plane& plane);
	bool isOBBSphereCollision(const OBB& obb, const Sphere& sphere);
	bool isOBBAABBCollision(const OBB& obb, const AABB& aabb);
	bool isOBBOBBCollision(const OBB& o1, const OBB& o2);

	void renderOBB(const OBB& obb, Shader& shader);

	//bool isOBBMeshCollision(const OBB& obb, const Mesh& mesh);
	//bool isOBBModelCollision(const OBB& obb, const Model& model);
}

/*
namespace pa
{
	struct Mesh
	{
		int32 numTriangles;

		union
		{
			std::vector<Triangle>	triangles;
			std::vector<Point>		points;
			std::vector<float>		values;
		};
	};

	bool isMeshTriangleCollision(const Mesh& mesh, const Triangle& triangle);
	bool isMeshPlaneCollision(const Mesh& mesh, const Plane& plane);
	bool isMeshSphereCollision(const Mesh& mesh, const Sphere& sphere);
	bool isMeshAABBCollision(const Mesh& mesh, const AABB& aabb);
	bool isMeshOBBCollision(const Mesh& mesh, const OBB& obb);
}

namespace pa
{
	class Model
	{
	protected :
		Mesh* _mesh;
		AABB  _boundary;

	public :
		glm::vec3	_position = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::quat	_rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
		Model*		_parent;

	public :
		Model(void) : _parent(0), _mesh(0) {}

		Mesh* getMesh(void) const { return _mesh; }
		AABB getBoundary(void) const { return _boundary; }
		OBB getOBB(void);
		glm::mat4 getWorldMatrix(void) const;

		void setMesh(Mesh* mesh);
	};
}
*/

namespace pa
{
	struct CollisionManifold
	{
		bool				isColliding;
		glm::vec3			normal;
		float				depth;
		std::vector<Point>	contacts;
	};

	void resetCollisionManifold(_Out_ CollisionManifold* manifold);

	void findCollisionManifold(const Sphere& s1, const Sphere& s2, _Out_ CollisionManifold* outManifold);
	void findCollisionManifold(const OBB& obb, const Sphere& sphere, _Out_ CollisionManifold* outManifold);
	void findCollisionManifold(const OBB& o1, const OBB& o2, _Out_ CollisionManifold* outManifold);
}