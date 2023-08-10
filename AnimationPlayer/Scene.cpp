//#include <algorithm>
//#include <stack>
//
//#include "Scene.h"
//
//namespace pa
//{
//#pragma region Octree
//	void splitOctree(OctreeNode* node, int32 depth)
//	{
//		if (depth <= 0)
//			return;
//
//		if (node->children == nullptr)
//		{
//			node->children = new OctreeNode[8];
//
//			glm::vec3 center = node->boundary.position;
//			glm::vec3 edge = node->boundary.size * 0.5f;
//
//			node->children[0].boundary = AABB(center + glm::vec3(-edge.x, edge.y, -edge.z), edge);
//			node->children[1].boundary = AABB(center + glm::vec3(edge.x, edge.y, -edge.z), edge);
//			node->children[2].boundary = AABB(center + glm::vec3(-edge.x, edge.y, edge.z), edge);
//			node->children[3].boundary = AABB(center + glm::vec3(edge.x, edge.y, edge.z), edge);
//			node->children[4].boundary = AABB(center + glm::vec3(-edge.x, -edge.y, -edge.z), edge);
//			node->children[5].boundary = AABB(center + glm::vec3(edge.x, -edge.y, -edge.z), edge);
//			node->children[6].boundary = AABB(center + glm::vec3(-edge.x, -edge.y, edge.z), edge);
//			node->children[7].boundary = AABB(center + glm::vec3(edge.x, -edge.y, edge.z), edge);
//		}
//
//		if (node->children != nullptr && node->models.size() > 0)
//		{
//			for (int32 i = 0; i < 8; ++i)
//			{
//				int32 n = node->models.size();
//				for (int32 j = 0; j < n; ++j)
//				{
//					OBB boundary = node->models[j]->getOBB();
//					if (isAABBOBBCollision(node->children[i].boundary, boundary) == true)
//						node->children[i].models.push_back(node->models[j]);
//				}
//			}
//			node->models.clear();
//
//			for (int32 i = 0; i < 8; ++i)
//				splitOctree(&(node->children[i]), depth - 1);
//		}
//	}
//
//	void insertOctree(OctreeNode* node, Model* model)
//	{
//		OBB boundary = model->getOBB();
//
//		if (isAABBOBBCollision(node->boundary, boundary) == true)
//		{
//			if (node->children == nullptr)
//				node->models.push_back(model);
//			else
//			{
//				for (int32 i = 0; i < 8; ++i)
//					insertOctree(&(node->children[i]), model);
//			}
//		}
//	}
//
//	void removeOctree(OctreeNode* node, Model* model)
//	{
//		if (node->children == nullptr)
//		{
//			std::vector<Model*>::iterator iter = std::find(node->models.begin(), node->models.end(), model);
//			if (iter != node->models.end())
//				node->models.erase(iter);
//		}
//		else
//		{
//			for (int32 i = 0; i < 8; ++i)
//				removeOctree(&(node->children[i]), model);
//		}
//	}
//
//	void updateOctree(OctreeNode* node, Model* model)
//	{
//		removeOctree(node, model);
//		insertOctree(node, model);
//	}
//
//	Model* findClosestModel(const Ray& ray, const std::vector<Model*>& set)
//	{
//		if (set.size() == 0)
//			return nullptr;
//
//		Model* closest = nullptr;
//		float rayTime = -1.0f;
//
//		int32 n = set.size();
//		for (int32 i = 0; i < n; ++i)
//		{
//			RaycastInfo raycastInfo;
//			raycast(ray, *(set[i]), &raycastInfo);
//
//			if (raycastInfo.isHit == false)
//				continue;
//
//			if (rayTime < 0.0f || raycastInfo.rayTime < rayTime)
//			{
//				closest = set[i];
//				rayTime = raycastInfo.rayTime;
//			}
//		}
//
//		return closest;
//	}
//
//	Model* raycast(const Ray& ray, OctreeNode* node)
//	{
//		RaycastInfo raycastInfo;
//		raycast(ray, node->boundary, &raycastInfo);
//
//		if (raycastInfo.isHit == true)
//		{
//			if (node->children == nullptr)
//				return findClosestModel(ray, node->models);
//			else
//			{
//				std::vector<Model*> candidates;
//				for (int32 i = 0; i < 8; ++i)
//				{
//					Model* casted = raycast(ray, &(node->children[i]));
//					if (casted != nullptr)
//						candidates.push_back(casted);
//				}
//				return findClosestModel(ray, candidates);
//			}
//		}
//
//		return nullptr;
//	}
//
//	std::vector<Model*> query(OctreeNode* node, const Sphere& sphere)
//	{
//		std::vector<Model*> contained;
//
//		if (isSphereAABBCollision(sphere, node->boundary) == true)
//		{
//			if (node->children == nullptr)
//			{
//				int32 n = node->models.size();
//				for (int32 i = 0; i < n; ++i)
//				{
//					OBB boundary = node->models[i]->getOBB();
//					if (isSphereOBBCollision(sphere, boundary) == true)
//						contained.push_back(node->models[i]);
//				}
//			}
//			else
//			{
//				for (int32 i = 0; i < 8; ++i)
//				{
//					std::vector<Model*> child = query(&(node->children[i]), sphere);
//					if (child.size() > 0)
//						contained.insert(contained.end(), child.begin(), child.end());
//				}
//			}
//		}
//		
//		return contained;
//	}
//
//	std::vector<Model*> query(OctreeNode* node, const AABB& aabb)
//	{
//		std::vector<Model*> contained;
//
//		if (isAABBAABBCollision(aabb, node->boundary) == true)
//		{
//			if (node->children == nullptr)
//			{
//				int32 n = node->models.size();
//				for (int32 i = 0; i < n; ++i)
//				{
//					OBB boundary = node->models[i]->getOBB();
//					if (isAABBOBBCollision(aabb, boundary) == true)
//						contained.push_back(node->models[i]);
//				}
//			}
//			else
//			{
//				for (int32 i = 0; i < 8; ++i)
//				{
//					std::vector<Model*> child = query(&(node->children[i]), aabb);
//					if (child.size() > 0)
//						contained.insert(contained.end(), child.begin(), child.end());
//				}
//			}
//		}
//
//		return contained;
//	}
//#pragma endregion
//
//
//
//#pragma region Scene
//	void Scene::addModel(Model* model)
//	{
//		if (std::find(objects.begin(), objects.end(), model) != objects.end())
//			return;
//
//		objects.push_back(model);
//	}
//
//	void Scene::removeModel(Model* model)
//	{
//		objects.erase(std::remove(objects.begin(), objects.end(), model), objects.end());
//	}
//
//	void Scene::updateModel(Model* model)
//	{
//		__noop;
//	}
//
//	std::vector<Model*> Scene::findChildren(const Model* model)
//	{
//		std::vector<Model*> children;
//
//		int32 n = objects.size();
//		for (int32 i = 0; i < n; ++i)
//		{
//			if (objects[i] == nullptr || objects[i] == model)
//				continue;
//
//			Model* iterator = objects[i]->_parent;
//			//Recheck Here
//			if (iterator != nullptr)
//			{
//				if (iterator == model)
//				{
//					children.push_back(objects[i]);
//					continue;
//				}
//				iterator = iterator->_parent;
//			}
//		}
//		return children;
//	}
//
//	Model* Scene::raycast(const Ray& ray)
//	{
//		if (octree != nullptr)
//			return pa::raycast(ray, octree);
//
//		Model* closest = nullptr;
//		float rayTime = -1.0f;
//
//		int32 n = objects.size();
//		for (int32 i = 0; i < n; ++i)
//		{
//			RaycastInfo raycastInfo;
//			pa::raycast(ray, *objects[i], &raycastInfo);
//			if (closest == nullptr && raycastInfo.isHit == true)
//			{
//				closest = objects[i];
//				rayTime = raycastInfo.rayTime;
//			}
//			else if (closest != nullptr && raycastInfo.rayTime < rayTime)
//			{
//				closest = objects[i];
//				rayTime = raycastInfo.rayTime;
//			}
//		}
//		return closest;
//	}
//
//	std::vector<Model*> Scene::query(const Sphere& sphere)
//	{
//		if (octree != nullptr)
//			return pa::query(octree, sphere);
//
//		std::vector<Model*> contained;
//
//		int32 n = objects.size();
//		for (int32 i = 0; i < n; ++i)
//		{
//			OBB boundary = objects[i]->getOBB();
//			if (isSphereOBBCollision(sphere, boundary) == true)
//				contained.push_back(objects[i]);
//		}
//		return contained;
//	}
//
//	std::vector<Model*> Scene::query(const AABB& aabb)
//	{
//		if (octree != nullptr)
//			return pa::query(octree, aabb);
//
//		std::vector<Model*> contained;
//
//		int32 n = objects.size();
//		for (int32 i = 0; i < n; ++i)
//		{
//			OBB boundary = objects[i]->getOBB();
//			if (isAABBOBBCollision(aabb, boundary) == true)
//				contained.push_back(objects[i]);
//		}
//		return contained;
//	}
//
//	bool Scene::accelerate(const glm::vec3& position, float size)
//	{
//		if (octree != nullptr)
//			return false;
//
//		glm::vec3 min(position.x - size, position.y - size, position.z - size);
//		glm::vec3 max(position.x + size, position.y + size, position.z + size);
//
//		octree = new OctreeNode();
//		octree->boundary = makeAABBFromMinMax(min, max);
//		octree->children = nullptr;
//
//		int32 n = objects.size();
//		for (int32 i = 0; i < n; ++i)
//			octree->models.push_back(objects[i]);
//
//		splitOctree(octree, 5);
//		return true;
//	}
//#pragma endregion
//}