#pragma once
#include <vector>

#include "Geometry3D.h"

namespace pa
{
	struct OctreeNode
	{
		OctreeNode(void) : children(0) { }
		~OctreeNode(void)
		{
			if (children != nullptr)
				delete[] children;
		}

		AABB bounds;
		OctreeNode* children;
		std::vector<Model*> models;
	};

	void splitTree(OctreeNode* node, int32 depth);
	void insertTree(OctreeNode* node, Model* model);
	void removeTree(OctreeNode* node, Model* model);
	void updateTree(OctreeNode* node, Model* model);

	Model* findClosestModel(const Ray& ray, const std::vector<Model*>& set);
	Model* raycast(const Ray& ray, OctreeNode* node);
	std::vector<Model*> query(OctreeNode* node, const Sphere& sphere);
	std::vector<Model*> query(OctreeNode* node, const AABB& aabb);
}

namespace pa
{
	class Scene
	{
	protected:
		std::vector<Model*> objects;
		OctreeNode*			octree = nullptr;

	private:
		Scene(const Scene&);
		Scene& operator= (const Scene&);

	public:
		Scene(void) : octree(0) { }
		~Scene(void) {
			if (octree != nullptr)
				delete octree;
		}

		void addModel(Model* model);
		void removeModel(Model* model);
		void updateModel(Model* model);
		std::vector<Model*> findChildren(const Model* model);

		Model* raycast(const Ray& ray);
		std::vector<Model*> query(const Sphere& sphere);
		std::vector<Model*> query(const AABB& aabb);

		bool accelerate(const glm::vec3& position, float size);
	};
}

