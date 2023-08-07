#include <algorithm>
#include <stack>

#include "Scene.h"

namespace pa
{
#pragma region Octree
	void splitTree(OctreeNode* node, int32 depth)
	{
		if (depth <= 0)
			return;

		if (node->children == nullptr)
		{
			node->children = new OctreeNode[8];

			glm::vec3 center = node->bounds.position;
			glm::vec3 edge = node->bounds.size * 0.5f;

			node->children[0].bounds = AABB(center + glm::vec3(-edge.x, edge.y, -edge.z), edge);
			node->children[1].bounds = AABB(center + glm::vec3(edge.x, edge.y, -edge.z), edge);
			node->children[2].bounds = AABB(center + glm::vec3(-edge.x, edge.y, edge.z), edge);
			node->children[3].bounds = AABB(center + glm::vec3(edge.x, edge.y, edge.z), edge);
			node->children[4].bounds = AABB(center + glm::vec3(-edge.x, -edge.y, -edge.z), edge);
			node->children[5].bounds = AABB(center + glm::vec3(edge.x, -edge.y, -edge.z), edge);
			node->children[6].bounds = AABB(center + glm::vec3(-edge.x, -edge.y, edge.z), edge);
			node->children[7].bounds = AABB(center + glm::vec3(edge.x, -edge.y, edge.z), edge);
		}

		if (node->children != nullptr && node->models.size() > 0)
		{
			for (int32 i = 0; i < 8; ++i)
			{
				int32 n = node->models.size();
				for (int32 j = 0; j < n; ++j)
				{
					OBB bounds = node->models[j]->getOBB();
					if (isAABBOBBCollision(node->children[i].bounds, bounds) == true)
						node->children[i].models.push_back(node->models[j]);
				}
			}
			node->models.clear();

			for (int32 i = 0; i < 8; ++i)
				splitTree(&(node->children[i]), depth - 1);
		}
	}

	void insertTree(OctreeNode* node, Model* model)
	{
		OBB bounds = model->getOBB();

		if (isAABBOBBCollision(node->bounds, bounds) == true)
		{
			if (node->children == nullptr)
				node->models.push_back(model);
			else
			{
				for (int32 i = 0; i < 8; ++i)
					insertTree(&(node->children[i]), model);
			}
		}
	}

	void removeTree(OctreeNode* node, Model* model)
	{
		if (node->children == nullptr)
		{
			std::vector<Model*>::iterator iter = std::find(node->models.begin(), node->models.end(), model);
			if (iter != node->models.end())
				node->models.erase(iter);
		}
		else
		{
			for (int32 i = 0; i < 8; ++i)
				removeTree(&(node->children[i]), model);
		}
	}

	void updateTree(OctreeNode* node, Model* model)
	{
		removeTree(node, model);
		insertTree(node, model);
	}

	Model* findClosestModel(const Ray& ray, const std::vector<Model*>& set)
	{
		if (set.size() == 0)
			return nullptr;

		Model* closest = nullptr;
		float rayTime = -1.0f;

		int32 n = set.size();
		for (int32 i = 0; i < n; ++i)
		{
			float t = set[i]->isRayCollision(ray);

			if (t < 0.0f)
				continue;

			if (rayTime < 0.0f || t < rayTime)
			{
				closest = set[i];
				rayTime = t;
			}
		}

		return closest;
	}

	Model* raycast(const Ray& ray, OctreeNode* node)
	{
		float rayTime = raycast(ray, node->bounds);

		if (rayTime >= 0.0f)
		{
			if (node->children == nullptr)
				return findClosestModel(ray, node->models);
			else
			{
				std::vector<Model*> candidates;
				for (int32 i = 0; i < 8; ++i)
				{
					Model* casted = raycast(ray, &(node->children[i]));
					if (casted != nullptr)
						candidates.push_back(casted);
				}
				return findClosestModel(ray, candidates);
			}
		}

		return nullptr;
	}

	std::vector<Model*> query(OctreeNode* node, const Sphere& sphere)
	{
		std::vector<Model*> collided;

		if (isSphereAABBCollision(sphere, node->bounds) == true)
		{
			if (node->children == nullptr)
			{
				int32 n = node->models.size();
				for (int32 i = 0; i < n; ++i)
				{
					OBB bounds = node->models[i]->getOBB();
					if (isSphereOBBCollision(sphere, bounds) == true)
						collided.push_back(node->models[i]);
				}
			}
			else
			{
				for (int32 i = 0; i < 8; ++i)
				{
					std::vector<Model*> child = query(&(node->children[i]), sphere);
					if (child.size() > 0)
						collided.insert(collided.end(), child.begin(), child.end());
				}
			}
		}
		
		return collided;
	}

	std::vector<Model*> query(OctreeNode* node, const AABB& aabb)
	{
		std::vector<Model*> collided;

		if (isAABBAABBCollision(aabb, node->bounds) == true)
		{
			if (node->children == nullptr)
			{
				int32 n = node->models.size();
				for (int32 i = 0; i < n; ++i)
				{
					OBB bounds = node->models[i]->getOBB();
					if (isAABBOBBCollision(aabb, bounds) == true)
						collided.push_back(node->models[i]);
				}
			}
			else
			{
				for (int32 i = 0; i < 8; ++i)
				{
					std::vector<Model*> child = query(&(node->children[i]), aabb);
					if (child.size() > 0)
						collided.insert(collided.end(), child.begin(), child.end());
				}
			}
		}

		return collided;
	}
#pragma endregion



#pragma region Scene
	void Scene::addModel(Model* model)
	{
		if (std::find(objects.begin(), objects.end(), model) != objects.end())
			return;

		objects.push_back(model);
	}

	void Scene::removeModel(Model* model)
	{
		objects.erase(std::remove(objects.begin(), objects.end(), model), objects.end());
	}

	void Scene::updateModel(Model* model)
	{
		__noop;
	}

	std::vector<Model*> Scene::findChildren(const Model* model)
	{
		std::vector<Model*> result;

		int32 n = objects.size();
		for (int32 i = 0; i < n; ++i)
		{
			if (objects[i] == nullptr || objects[i] == model)
				continue;

			Model* iterator = objects[i]->parent;
			//Recheck Here
			if (iterator != nullptr)
			{
				if (iterator == model)
				{
					result.push_back(objects[i]);
					continue;
				}
				iterator = iterator->parent;
			}
		}
		return result;
	}

	Model* Scene::raycast(const Ray& ray)
	{
		if (octree != nullptr)
			return pa::raycast(ray, octree);

		Model* closest = nullptr;
		float rayTime = -1.0f;

		int32 n = objects.size();
		for (int32 i = 0; i < n; ++i)
		{
			float t = objects[i]->isRayCollision(ray);
			if (closest == nullptr && t >= 0.0f)
			{
				closest = objects[i];
				rayTime = t;
			}
			else if (closest != nullptr && t < rayTime)
			{
				closest = objects[i];
				rayTime = t;
			}
		}
		return closest;
	}

	std::vector<Model*> Scene::query(const Sphere& sphere)
	{
		if (octree != nullptr)
			return pa::query(octree, sphere);

		std::vector<Model*> collided;

		int32 n = objects.size();
		for (int32 i = 0; i < n; ++i)
		{
			OBB bounds = objects[i]->getOBB();
			if (isSphereOBBCollision(sphere, bounds) == true)
				collided.push_back(objects[i]);
		}
		return collided;
	}

	std::vector<Model*> Scene::query(const AABB& aabb)
	{
		if (octree != nullptr)
			return pa::query(octree, aabb);

		std::vector<Model*> collided;

		int32 n = objects.size();
		for (int32 i = 0; i < n; ++i)
		{
			OBB bounds = objects[i]->getOBB();
			if (isAABBOBBCollision(aabb, bounds) == true)
				collided.push_back(objects[i]);
		}
		return collided;
	}

	bool Scene::accelerate(const glm::vec3& position, float size)
	{
		if (octree != nullptr)
			return false;

		glm::vec3 min(position.x - size, position.y - size, position.z - size);
		glm::vec3 max(position.x + size, position.y + size, position.z + size);

		octree = new OctreeNode();
		octree->bounds = makeAABBFromMinMax(min, max);
		octree->children = nullptr;

		int32 n = objects.size();
		for (int32 i = 0; i < n; ++i)
			octree->models.push_back(objects[i]);

		splitTree(octree, 5);
		return true;
	}
#pragma endregion
}