#pragma once

class BVH
{
public:

	BVH(std::vector<std::shared_ptr<Intersectable>>& geo, int maxPrims)
	{
		this->maxPrims = maxPrims;
		this->geo = geo;

		std::vector<Primitive> prim(geo.size());
		for (INT i = 0; i < geo.size(); i++)
		{
			XMFLOAT3 pMin, pMax;
			geo[i]->CalculateAABB(pMin, pMax);
			prim[i] = { i , pMin, pMax };
		}
		std::vector<BVHNode> BVHNodes(2 * geo.size() - 1);
		

		int currentNode = 0;
		std::vector<std::shared_ptr<Intersectable>> orderedPrims;
		BVHNode* root;
		root = recursiveBuild(BVHNodes, prim, 0, geo.size(), &currentNode, orderedPrims);
		
		geo.swap(orderedPrims);
		fastBVHNodes = new std::vector<CacheFriendlyBVHNode>(BVHNodes.size());
		int offset = 0;
		FlattenBVHTree(root, &offset);
		int x = 0;
	}


	bool Intersect(Ray& ray, const Intersectable*& object)
	{
		XMFLOAT3 invRayDir;
		XMStoreFloat3(&invRayDir, ray.direction);
		invRayDir = { 1.0f / invRayDir.x, 1.0f / invRayDir.y, 1.0f / invRayDir.z };
		XMINT3 isDirNeg = { invRayDir.x < 0, invRayDir.y < 0, invRayDir.z < 0 };
		std::vector<int> indexStack(128);
		int indexOffset = 0;
		int currentIndex = 0;
		bool intersectionFound = false;
		while(true)
		{
			CacheFriendlyBVHNode* currentNode = &fastBVHNodes->at(currentIndex);
			if (currentNode->Intersect(ray, isDirNeg, invRayDir))
			{
				if (currentNode->count > 0)
				{
					for (int i = currentNode->first; i < currentNode->first + currentNode->count; i++)
					{
						if (geo[i]->Intersect(ray))
						{
							object = geo[i].get();
							intersectionFound = true;
						}
					}
					if (indexOffset == 0) break;
					
					currentIndex = indexStack[--indexOffset];
				}
				else
				{
					int axis = currentNode->axis;
					int isDirNegToAxis;
					if (axis == 0)
						isDirNegToAxis = isDirNeg.x;
					else if (axis == 1)
						isDirNegToAxis = isDirNeg.y;
					else
						isDirNegToAxis = isDirNeg.z;
			
					if (isDirNegToAxis)
					{
						indexStack[indexOffset++] = currentIndex + 1;
						currentIndex = currentNode->secondChildOffset;
					}
					else
					{
						indexStack[indexOffset++] = currentNode->secondChildOffset;
						currentIndex++;
					}
				}
			}
			else
			{
				if (indexOffset == 0) break;
				currentIndex = indexStack[--indexOffset];
			}
		}
		return intersectionFound;
	}


	
private:
	struct Primitive
	{
		Primitive() {}
		Primitive(int n, XMFLOAT3 pMin, XMFLOAT3 pMax)
		{
			index = n;
			this->pMax = pMax;
			this->pMin = pMin;
			XMStoreFloat3(&center, 0.5 * (XMLoadFloat3(&pMax) + XMLoadFloat3(&pMin)));
		}

		INT index;
		XMFLOAT3 pMin, pMax, center;


	};
	struct CacheFriendlyBVHNode
	{
		XMFLOAT3 pMin, pMax;
		union
		{
			int first;
			int secondChildOffset;
		};

		UINT16 count;
		UINT8 axis;
		UINT8 pad[1];

		bool Intersect(const Ray& ray, const XMINT3& isDirNeg, const XMFLOAT3& invRayDir)
		{
			XMFLOAT3 PMin = { isDirNeg.x ? pMax.x : pMin.x, isDirNeg.y ? pMax.y : pMin.y, isDirNeg.z ? pMax.z : pMin.z };
			XMFLOAT3 PMax = { !isDirNeg.x ? pMax.x : pMin.x, !isDirNeg.y ? pMax.y : pMin.y, !isDirNeg.z ? pMax.z : pMin.z };
			float tXMin = (PMin.x - XMVectorGetX(ray.origin)) * invRayDir.x;
			float tXMax = (PMax.x - XMVectorGetX(ray.origin)) * invRayDir.x;

			float tYMin = (PMin.y - XMVectorGetY(ray.origin)) * invRayDir.y;
			float tYMax = (PMax.y - XMVectorGetY(ray.origin)) * invRayDir.y;

			if (tXMin > tYMax || tYMin > tXMax)
				return false;
			if (tYMin > tXMin) tXMin = tYMin;
			if (tYMax < tXMax) tXMax = tYMax;

			float tZMin = (PMin.z - XMVectorGetZ(ray.origin)) * invRayDir.z;
			float tZMax = (PMax.z - XMVectorGetZ(ray.origin)) * invRayDir.z;

			if (tXMin > tZMax || tZMin > tXMax)
				return false;

			if (tZMin > tXMin)
				tXMin = tZMin;
			if (tZMax < tXMax)
				tXMax = tZMax;

			return (tXMin < ray.t) && (tXMax > 0);



		}

	};

	struct BVHNode
	{
	public:
		BVHNode(){}
		
		void CreateAABB(XMFLOAT3& aPMin, XMFLOAT3& aPMax, XMFLOAT3& bPMin, XMFLOAT3& bPMax)
		{
			pMin = {
				min(aPMin.x, bPMin.x),
				min(aPMin.y, bPMin.y),
				min(aPMin.z, bPMin.z),
			};
			pMax = {
				max(aPMax.x, bPMax.x),
				max(aPMax.y, bPMax.y),
				max(aPMax.z, bPMax.z),
			};

		}
		

		void Leaf(int f, int n, XMFLOAT3& pMin, XMFLOAT3& pMax)
		{
			first = f;
			count = n;
			this->pMin = pMin;
			this->pMax = pMax;
			left = nullptr;
			right = nullptr;
		}

		void Node(int axis, BVHNode* left, BVHNode* right)
		{
			this->left = left;
			this->right = right;
			CreateAABB(left->pMin, left->pMin, right->pMin, right->pMax);
			this->axis = axis;
			count = 0;

		}

		XMFLOAT3 pMin, pMax;
		BVHNode* left;
		BVHNode* right;
		int axis, first, count;
	};
	
	BVHNode* recursiveBuild(std::vector<BVHNode>& nodes, std::vector<Primitive>& prim, int start, int end, int* currentNode, std::vector<std::shared_ptr<Intersectable>>& orderedPrims)
	{
		int nodeIndex = (*currentNode);
		(*currentNode)++;
		
		XMFLOAT3 pMin = prim[start].pMin;
		XMFLOAT3 pMax = prim[start].pMax;
		XMFLOAT3 totalPMin, totalPMax;
		for (int i = start + 1; i < end; i++)
		{
			CreateAABB(totalPMin, totalPMax, pMin, pMax, prim[i].pMin, prim[i].pMax);
			pMin = totalPMin;
			pMax = totalPMax;
		}
		int count = end - start;
		if (count == 1)
		{
			int first = orderedPrims.size();
			for (int i = start; i < end; i++)
			{
				int index = prim[i].index;
				orderedPrims.push_back(geo[index]);
			}
			/*nodes[*currentNode] = new BVHNode();*/
			nodes[nodeIndex].Leaf(first, count, pMin, pMax);
			return &nodes[nodeIndex];
		}
		else
		{
			XMFLOAT3 centerPMin = prim[start].center;
			XMFLOAT3 centerPMax = prim[start].center;
			for (int i = start + 1; i < end; i++)
			{
				CreateAABB(totalPMin, totalPMax, centerPMin, centerPMax, prim[i].center, prim[i].center);
				centerPMin = totalPMin;
				centerPMax = totalPMax;
			}
			XMFLOAT3 range = { centerPMax.x - centerPMin.x, centerPMax.y - centerPMin.y, centerPMax.z - centerPMin.z };
			int dim = (range.x > range.y && range.x > range.z) ? 0 : range.y > range.z ? 1 : 2;
			int mid = (start + end) / 2;
			float cMin = 0.5f * centerPMin.x * (dim - 1) * (dim - 2) + centerPMin.y * dim * (2 - dim) + 0.5f *centerPMin.z * dim * (dim - 1);
			float cMax = 0.5f * centerPMax.x * (dim - 1) * (dim - 2) + centerPMax.y * dim * (2 - dim) + 0.5f * centerPMax.z * dim * (dim - 1);
			if (cMin == cMax)
			{
				int first = orderedPrims.size();
				for (int i = start; i < end; i++)
				{
					int index = prim[i].index;
					orderedPrims.push_back(geo[index]);
				}
				nodes[nodeIndex].Leaf(first, count, pMin, pMax);
				return &nodes[nodeIndex];
			}
			else
			{
				float pMid = 0.5f * (cMin + cMax);
				Primitive* midPtr = std::partition(&prim[start], &prim[end - 1] + 1,
					[dim, pMid](const Primitive& pi) {
					return 0.5f * pi.center.x * (dim - 1) * (dim - 2) + pi.center.y * dim * (2 - dim) + 0.5f * pi.center.z * dim * (dim - 1) < pMid; });
				mid = midPtr - &prim[0];
				if (mid == start || mid == end)
				{
					mid = (start + end) / 2;
					std::nth_element(&prim[start], &prim[mid], &prim[end - 1] + 1,
						[dim](const Primitive& a, const Primitive& b)
					{return 0.5f * a.center.x * (dim - 1) * (dim - 2) + a.center.y * dim * (2 - dim) + 0.5f * a.center.z * dim * (dim - 1) <
						0.5f * b.center.x * (dim - 1) * (dim - 2) + b.center.y * dim * (2 - dim) + 0.5f * b.center.z * dim * (dim - 1); });
				}

				nodes[nodeIndex].Node(dim, recursiveBuild(nodes, prim, start, mid, currentNode, orderedPrims),
					recursiveBuild(nodes, prim, mid, end, currentNode, orderedPrims));
			}


		}
		return &nodes[nodeIndex];
	}

	int FlattenBVHTree(BVHNode* node, int* offset)
	{
		CacheFriendlyBVHNode* cachedBVH = &fastBVHNodes->at(*offset);
		cachedBVH->pMax = node->pMax;
		cachedBVH->pMin = node->pMin;
		int nextOffset = (*offset)++;
		if (node->count > 0)
		{
			cachedBVH->first = node->first;
			cachedBVH->count = node->count;
		}
		else
		{
			cachedBVH->axis = node->axis;
			cachedBVH->count = 0;
			FlattenBVHTree(node->left, offset);
			cachedBVH->secondChildOffset = FlattenBVHTree(node->right, offset);
		}
		return nextOffset;
	}
	void CreateAABB(XMFLOAT3& outPMin, XMFLOAT3& outPMax, XMFLOAT3& aPMin, XMFLOAT3& aPMax, XMFLOAT3& bPMin, XMFLOAT3& bPMax)
	{
		outPMin = {
			min(aPMin.x, bPMin.x),
			min(aPMin.y, bPMin.y),
			min(aPMin.z, bPMin.z),
		};
		outPMax = {
			max(aPMax.x, bPMax.x),
			max(aPMax.y, bPMax.y),
			max(aPMax.z, bPMax.z),
		};

	}
    int maxPrims;
	std::vector<std::shared_ptr<Intersectable>> geo;
	std::vector<CacheFriendlyBVHNode>* fastBVHNodes;
};
