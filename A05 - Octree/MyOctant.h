#ifndef _OCTANTCLASS_H_
#define _OCTANTCLASS_H_

#include "MyEntityManager.h"

namespace Simplex
{

class MyOctant
{
	static uint m_uOctantCount; //number of octants
	static uint m_uMaxLevel; //max octant level
	static uint m_uIdealEntityCount; //how many entities the octant contains

	uint m_uID = 0; //octant ID
	uint m_uLevel = 0; //octant level
	uint m_uChildren = 0; //octant's children

	float m_fSize = 0.0f; //octant size

	//Singletons
	MeshManager* m_pMeshMngr = nullptr; //mesh manager
	MyEntityManager* m_pEntityMngr = nullptr; //entity manager

	vector3 m_v3Center = vector3(0.0f); //octant center
	vector3 m_v3Min = vector3(0.0f); //octant min
	vector3 m_v3Max = vector3(0.0f); //octant max

	MyOctant* m_pParent = nullptr; //octant's parent nodes
	MyOctant* m_pChild[8]; //octant's children nodes

	std::vector<uint> m_EntityList; //entity manager list

	MyOctant* m_pRoot = nullptr; //root
	std::vector<MyOctant*> m_lChild; //root nodes



	public:
		//constructors
		MyOctant(uint a_maxLevel = 2, uint a_nIdealEntityCount = 5);
		MyOctant(vector3 a_v3Center, float a_fSize);
		MyOctant(MyOctant const& other);
		MyOctant& operator=(MyOctant const& other);
		//destructor for cleanup
		~MyOctant(void);

		//swaps octant with other octant
		void Swap(MyOctant& other);

		//Get Methods
		float GetSize(void);

		vector3 GetCenterGlobal(void);

		vector3 GetMinGlobal(void);

		vector3 GetMaxGlobal(void);

		MyOctant* GetChild(uint a_nChild);

		MyOctant* GetParent(void);

		//determines if there is collision
		bool IsColliding(uint a_uRBIndex);

		//displaying octree
		void Display(uint a_nIndex, vector3 a_v3Color = C_YELLOW);
		void Display(vector3 a_v3Color = C_YELLOW);

		//displaying octree leaves
		void DisplayLeafs(vector3 a_v3Color = C_YELLOW);

		void ClearEntityList(void);

		//dividing octree into sections
		void Subdivide(void);

		bool IsLeaf(void);

		bool ContainsMoreThan(uint a_nEntities);

		void KillBranches(void);

		void ConstructTree(u_int a_nMaxLevel = 3);

		void AssignIDtoEntity(void);

		uint GetOctantCount(void);

	private:
		void Release(void); //destroys octree

		void Init(void); //creates octree

		void ConstructList(void); //recursive construction of list

	};

}

#endif