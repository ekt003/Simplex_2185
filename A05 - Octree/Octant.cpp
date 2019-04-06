#include "Octant.h"
using namespace Simplex;

uint Octant::m_uOctantCount = 0;
uint Octant::m_uMaxLevel = 3;
uint Octant::m_uIdealEntityCount = 5;
uint Octant::GetOctantCount(void) { return m_uOctantCount; }

void Octant::Init(void) {
	m_uChildren = 0;
	m_fSize = 0.0f;
	m_uID = m_uOctantCount;
	m_uLevel = 0;
	
	m_v3Center = vector3(0.0f);
	m_v3Min = vector3(0.0f);
	m_v3Max = vector3(0.0f);

	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = MyEntityManager::GetInstance();

	m_pRoot = nullptr;
	m_pParent = nullptr;

	for (int i = 0; i < 8; i++) {
		m_pChild[i] = nullptr;
	}

}

void Octant::ConstructList(void)
{
	for (int i = 0; i < m_uChildren; i++) {
		m_pChild[i]->ConstructList();
	}

	if (m_EntityList.size() > 0)
		m_pRoot->m_lChild.push_back(this);
}

void Octant::Swap(Octant & other)
{
	std::swap(m_uChildren, other.m_uChildren);

	std::swap(m_fSize, other.m_fSize);
	std::swap(m_uID, other.m_uID);
	std::swap(m_pRoot, other.m_pRoot);
	std::swap(m_lChild, other.m_lChild);

	std::swap(m_v3Center, other.m_v3Center);
	std::swap(m_v3Min, other.m_v3Min);
	std::swap(m_v3Max, other.m_v3Max);

	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = MyEntityManager::GetInstance();

	std::swap(m_uLevel, other.m_uLevel);
	std::swap(m_pParent, other.m_pParent);

	for (int i = 0; i < 8; i++) {
		std::swap(m_pChild[i], other.m_pChild[i]);
	}
}

void Octant::Release(void)
{
	if (m_uLevel == 0)
		KillBranches();

	m_uChildren = 0;
	m_fSize = 0.0f;
	m_EntityList.clear();
	m_lChild.clear();
}

float Octant::GetSize(void)
{
	return m_fSize;
}

vector3 Octant::GetCenterGlobal(void)
{
	return m_v3Center;
}

vector3 Octant::GetMinGloval(void)
{
	return m_v3Min;
}

vector3 Octant::GetMaxGlobal(void)
{
	return m_v3Max;
}

bool Octant::IsColliding(uint a_uRBIndex)
{
	uint nObjectCount = m_pEntityMngr->GetEntityCount();

	if (a_uRBIndex >= nObjectCount)
		return false;

	MyEntity* pEntity = m_pEntityMngr->GetEntity(a_uRBIndex);
	MyRigidBody* pRigidBody = pEntity->GetRigidBody();
	vector3 v3MinO = pRigidBody->GetMinGlobal();
	vector3 v3MaxO = pRigidBody->GetMaxGlobal();

	//Hey remember that sweet sweet AABB?

	//checking x axis collisions
	if (m_v3Max.x < v3MinO.x)
		return false;
	if (m_v3Min.x > v3MaxO.x)
		return false;

	//checking y axis collisions
	if (m_v3Max.y < v3MinO.y)
		return false;
	if (m_v3Min.y > v3MaxO.y)
		return false;

	//checking z axis collisions
	if (m_v3Max.z < v3MinO.z)
		return false;
	if (m_v3Min.z > v3MaxO.z)
		return false;

	return true;

}

void Octant::Display(uint a_nIndex, vector3 a_v3Color)
{
	if (m_uID == a_nIndex) {
		m_pMeshMngr->AddWireCubeToRenderList(glm::translate(IDENTITY_M4, m_v3Center) * glm::scale(vector3(m_fSize)), a_v3Color, RENDER_WIRE);
		return;
	}

	for (int i = 0; i < m_uChildren; i++) {
		m_pChild[i]->Display(a_nIndex);
	}
}

void Octant::Display(vector3 a_v3Color)
{
	for (int i = 0; i < m_uChildren; i++) {
		m_pChild[i]->Display(a_v3Color);
	}

	m_pMeshMngr->AddWireCubeToRenderList(glm::translate(IDENTITY_M4, m_v3Center) * glm::scale(vector3(m_fSize)), a_v3Color, RENDER_WIRE);
}

void Octant::DisplayLeafs(vector3 a_v3Color)
{
	uint leaves = m_lChild.size();
	for (int i = 0; i < leaves; i++) {
		m_lChild[i]->DisplayLeafs(a_v3Color);
	}

	m_pMeshMngr->AddWireCubeToRenderList(glm::translate(IDENTITY_M4, m_v3Center) * glm::scale(vector3(m_fSize)), a_v3Color, RENDER_WIRE);
}

void Octant::ClearEntityList(void)
{
	//clears not only our entity list but the entity list of our children
	for (int i = 0; i < m_uChildren; i++) {
		m_pChild[i]->ClearEntityList();
	}

	m_EntityList.clear();
}

void Octant::Subdivide(void)
{
	if (m_uLevel >= m_uMaxLevel)
		return;

	if (m_uChildren != 0)
		return;

	m_uChildren = 8;

	float fSize = m_fSize / 4.0f;
	float fSizeD = fSize * 2.0f;
	vector3 v3Center;

	//Child Number 1
	v3Center = m_v3Center;
	v3Center.x -= fSize;
	v3Center.y -= fSize;
	v3Center.z -= fSize;
	m_pChild[0] = new Octant(v3Center, fSizeD);

	//Child Number 2
	v3Center.x += fSizeD;
	m_pChild[1] = new Octant(v3Center, fSizeD);

	//Child Number 3
	v3Center.z += fSizeD;
	m_pChild[2] = new Octant(v3Center, fSizeD);

	//Child Number 4
	v3Center.x -= fSizeD;
	m_pChild[3] = new Octant(v3Center, fSizeD);

	//Child Number 5
	v3Center.y += fSizeD;
	m_pChild[4] = new Octant(v3Center, fSizeD);

	//Child Number 6
	v3Center.z -= fSizeD;
	m_pChild[5] = new Octant(v3Center, fSizeD);

	//Child Number 7
	v3Center.x += fSizeD;
	m_pChild[6] = new Octant(v3Center, fSizeD);

	//Child Number 8
	v3Center.z += fSizeD;
	m_pChild[7] = new Octant(v3Center, fSizeD);

	//Setting values for all the children
	for (int i = 0; i < 8; i++) {
		m_pChild[i]->m_pRoot = m_pRoot;
		m_pChild[i]->m_pParent = this;
		m_pChild[i]->m_uLevel = m_uLevel + 1;
		if (m_pChild[i]->ContainsMoreThan(m_uIdealEntityCount))
			m_pChild[i]->Subdivide();
	}

}

Octant * Octant::GetChild(uint a_nChild)
{
	if (a_nChild > 7)
		return nullptr;
	return m_pChild[a_nChild];
}

Octant * Octant::GetParent(void)
{
	return m_pParent;
}

bool Octant::IsLeaf(void)
{
	return m_uChildren == 8;
}

bool Octant::ContainsMoreThan(uint a_nEntities)
{
	uint count = 0;
	uint nObjectCount = m_pEntityMngr->GetEntityCount();
	for (int i = 0; i < nObjectCount; i++) {
		if (IsColliding(i))
			count++;
		if (count > a_nEntities)
			return true;
	}

	return false;
}

void Octant::KillBranches(void)
{
	for (int i = 0; i < m_uChildren; i++) {
		m_pChild[i]->KillBranches(); //recurses through all possible nodes and destroys all of them
		delete m_pChild[i];
		m_pChild[i] = nullptr;
	}

	m_uChildren = 0;

}

void Octant::ConstructTree(u_int a_nMaxLevel)
{
	//if it aint the root
	if (m_uLevel != 0)
		return;

	m_uMaxLevel = a_nMaxLevel;

	m_uOctantCount = 1;

	//make sure everything is clear and good to go before creation
	m_EntityList.clear();
	KillBranches();
	m_lChild.clear();

	if (ContainsMoreThan(m_uIdealEntityCount))
		Subdivide();

	AssignIDtoEntity();

	//create the tree!
	ConstructList();

}

void Octant::AssignIDtoEntity(void)
{
	for (int i = 0; i < m_uChildren; i++) {
		m_pChild[i]->AssignIDtoEntity();
	}

	if (m_uChildren == 0) {
		uint nEntities = m_pEntityMngr->GetEntityCount();
		for (int i = 0; i < nEntities; i++) {
			if (IsColliding(i)) {
				m_EntityList.push_back(i);
				m_pEntityMngr->AddDimension(i, m_uID);
			}

		}
	}
}

//Constructors
Octant::Octant(uint a_nMaxLevel, uint a_nIdealEntityCount)
{
	//Initialize all values
	Init();

	m_uOctantCount = 0;
	m_uMaxLevel = a_nMaxLevel;
	m_uIdealEntityCount = a_nIdealEntityCount;
	m_uID = m_uOctantCount;

	//setting octree root and children
	m_pRoot = this;
	m_lChild.clear();

	std::vector<vector3> lMinMax;

	uint nObjects = m_pEntityMngr->GetEntityCount();
	for (int i = 0; i < nObjects; i++) {
		MyEntity* pEntity = m_pEntityMngr->GetEntity[i];
		MyRigidBody* pRigidBody = new MyRigidBody(lMinMax);
		lMinMax.push_back(pRigidBody->GetMinGlobal());
		lMinMax.push_back(pRigidBody->GetMaxGlobal());
	}

	MyRigidBody* pRigidBody = new MyRigidBody(lMinMax);

	vector3 vHalfWidth = pRigidBody->GetHalfWidth();
	float fMax = vHalfWidth.x;
	for (int i = 0; i < 3; i++) {
		if (fMax < vHalfWidth[i])
			fMax = vHalfWidth[i];
	}

	vector3 v3Center = pRigidBody->GetCenterLocal();
	lMinMax.clear();
	SafeDelete(pRigidBody);

	m_fSize = fMax * 2.0f;
	m_v3Center = v3Center;
	m_v3Min = m_v3Center - (vector3(fMax));
	m_v3Max = m_v3Center + (vector3(fMax));

	m_uOctantCount++;

	ConstructTree(m_uMaxLevel);
}

Octant::Octant(vector3 a_v3Center, float a_fSize) {
	Init();
	m_v3Center = a_v3Center;
	m_fSize = a_fSize;

	m_v3Min = m_v3Center - (vector3(m_fSize) / 2.0f);
	m_v3Max = m_v3Center + (vector3(m_fSize) / 2.0f);

	m_uOctantCount++;
}

Octant::Octant(Octant const& other) {
	m_uChildren = other.m_uChildren;
	m_v3Center = other.m_v3Center;
	m_v3Min = other.m_v3Min;
	m_v3Max = other.m_v3Max;

	m_fSize = other.m_fSize;
	m_uID = other.m_uID;
	m_uLevel = other.m_uLevel;
	m_pParent = other.m_pParent;

	m_pRoot, other.m_pRoot;
	m_lChild, other.m_lChild;

	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = MyEntityManager::GetInstance();

	for (int i = 0; i < 8; i++) {
		m_pChild[i] = other.m_pChild[i];
	}

}

Octant& Octant::operator=(Octant const& other) {
	if (this != &other) {
		Release();
		Init();
		Octant temp(other);
		Swap(temp);
	}

	return *this;
}


Octant::~Octant()
{
	Release();
}


