#ifndef _OCTANTCLASS_H_
#define _OCTANTCLASS_H_

#include "MyEntityManager.h"

namespace Simplex
{

class Octant
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

	Octant* m_pParent = nullptr;
	Octant* m_pChild[8];

	std::vector<uint> m_EntityList;

	Octant* m_pRoot = nullptr;
	std::vector<Octant*> m_lChild;



	public:
		//constructors
		Octant(uint a_maxLevel = 2, uint a_nIdealEntityCount = 5);
		Octant(vector3 a_v3Center, float a_fSize);
		Octant(Octant const& other);
		Octant& operator=(Octant const& other);
		//destructor for cleanup
		~Octant(void);

		void Swap(Octant& other);

		float GetSize(void);

		vector3 GetCenterGlobal(void);

		vector3 GetMinGloval(void);

		vector3 GetMaxGlobal(void);

		bool IsColliding(uint a_uRBIndex);

		void Display(uint a_nIndex, vector3 a_v3Color = C_YELLOW);

		void Display(vector3 a_v3Color = C_YELLOW);

		void DisplayLeafs(vector3 a_v3Color = C_YELLOW);

		void ClearEntityList(void);

		void Subdivide(void);

		Octant* GetChild(uint a_nChild);

		Octant* GetParent(void);

		bool IsLeaf(void);

		bool ContainsMoreThan(uint a_nEntities);

		void KillBranches(void);

		void ConstructTree(u_int a_nMaxLevel = 3);

		void AssignIDtoEntity(void);

		uint GetOctantCount(void);

	private:
		void Release(void);

		void Init(void);

		void ConstructList(void);

	};

}

#endif