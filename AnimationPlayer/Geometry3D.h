#pragma once
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "CommonTypes.h"

typedef glm::vec3 Point;

struct Line
{

};

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

struct Plane
{
	Plane(void) { }
	Plane(const float d, const glm::vec3& n) : distance(d), normal(n) { }

	float		distance;
	glm::vec3	normal;
};

struct Ray
{
	Ray(void) : origin(0.0f, 0.0f, 0.0f), direction(0.0f, 0.0f, 1.0f) { }
	Ray(const glm::vec3& o, const glm::vec3& d) : origin(o), direction(glm::normalize(d)) { }

	glm::vec3 origin;
	glm::vec3 direction;
};

struct AABB
{
	AABB(void) : position(0.0f, 0.0f, 0.0f), size(1.0f, 1.0f, 1.0f) { }
	AABB(const glm::vec3& p, const glm::vec3& s) : position(p), size(s) { }

	glm::vec3 position;
	glm::vec3 size;
};

struct OBB
{
	OBB(void) : position(0.0f, 0.0f, 0.0f), size(1.0f, 1.0f, 1.0f) { }
	OBB(const glm::vec3& p, const glm::vec3& s) : position(p), size(s) { }
	OBB(const glm::vec3& p, const glm::vec3& s, const glm::mat3& m) : position(p), size(s), rotation(m) { }

	glm::vec3 position;
	glm::vec3 size;
	glm::mat3 rotation;
};

struct Sphere
{
	Sphere(void) : position(0.0f, 0.0f, 0.0f), radius(1.0f) { }
	Sphere(const glm::vec3& p, float r) : position(p), radius(r) { }

	glm::vec3	position;
	float		radius;
};

struct Cylinder
{
	Cylinder(void) : position(0.0f, 0.0f, 0.0f), direction(0.0f, 0.0f, 1.0f), radius(1.0f) { }
	Cylinder(const glm::vec3& p, const glm::vec3& d, float r) : position(p), direction(d), radius(r) { }

	glm::vec3	position;
	glm::vec3	direction;
	float		radius;
};

struct Mesh
{
	int32 numTriangles;

	union
	{
		std::vector<Triangle>	triangle;
		std::vector<Point>		points;
		float					values;
	};
};

class Model
{
protected :
	Mesh* _mesh;
	AABB  _bounds;

public :
	Model(void) : parent(0), _mesh(0) {}
	Mesh* GetMesh(void) const { return _mesh; }
	AABB GetBounds(void) const { return _bounds; }

	void SetMesh(Mesh* mesh);

	glm::vec3 position;
	glm::vec3 rotation;
	Model* parent;
};