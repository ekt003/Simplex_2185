#include "MyOctant.h"
using namespace Simplex;

uint MyOctant::m_uOctantCount = 0;
uint MyOctant::m_uMaxLevel = 3;
uint MyOctant::m_uIdealEntityCount = 5;

//Initializes octree
void MyOctant::Init(void) {
	//variable starting values
	m_uChildren = 0;
	m_fSize = 0.0f;
	m_uID = m_uOctantCount;
	m_uLevel = 0;
	
	//center, min, and max have not been calculated yet
	m_v3Center = vector3(0.0f);
	m_v3Min = vector3(0.0f);
	m_v3Max = vector3(0.0f);

	//Gets instances of Singletons
	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = MyEntityManager::GetInstance();

	//Octant relations have not been set yet
	m_pRoot = nullptr;
	m_pParent = nullptr;
	for (int i = 0; i < 8; i++) {
		m_pChild[i] = nullptr;
	}

}

//constructs lists for each child
void MyOctant::ConstructList(void)
{
	for (int i = 0; i < m_uChildren; i++) {
		m_pChild[i]->ConstructList();
	}

	if (m_EntityList.size() > 0)
		m_pRoot->m_lChild.push_back(this);
}

//swaps octant information with another given octant
void MyOctant::Swap(MyOctant & other)
{
	std::swap(m_v3Center, other.m_v3Center);
	std::swap(m_v3Max, other.m_v3Max);
	std::swap(m_v3Min, other.m_v3Min);

	std::swap(m_fSize, other.m_fSize);
	std::swap(m_uID, other.m_uID);
	std::swap(m_pRoot, other.m_pRoot);
	std::swap(m_lChild, other.m_lChild);

	std::swap(m_uChildren, other.m_uChildren);

	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = MyEntityManager::GetInstance();

	std::swap(m_uLevel, other.m_uLevel);
	std::swap(m_pParent, other.m_pParent);

	//swaps children
	for (int i = 0; i < 8; i++) {
		std::swap(m_pChild[i], other.m_pChild[i]);
	}
}

//destructor which can be used more than once at the end of the program
void MyOctant::Release(void)
{
	if (m_uLevel == 0)
		KillBranches();

	//resets variables
	m_uChildren = 0;
	m_fSize = 0.0f;
	m_EntityList.clear();
	m_lChild.clear();
}

//Size Getter
float MyOctant::GetSize(void)
{
	return m_fSize;
}

//Global Center Getter
vector3 MyOctant::GetCenterGlobal(void)
{
	return m_v3Center;
}

//Global Min Getter
vector3 MyOctant::GetMinGlobal(void)
{
	return m_v3Min;
}

//Global Max Getter
vector3 MyOctant::GetMaxGlobal(void)
{
	return m_v3Max;
}

//Octant Count Getter
uint MyOctant::GetOctantCount(void) 
{ 
	return m_uOctantCount; 
}

//determines if there is a collision
bool MyOctant::IsColliding(uint a_uRBIndex)
{
	uint nObjectCount = m_pEntityMngr->GetEntityCount();

	if (a_uRBIndex >= nObjectCount)
		return false;

	MyEntity* pEntity = m_pEntityMngr->GetEntity(a_uRBIndex);
	MyRigidBody* pRigidBody = pEntity->GetRigidBody();
	vector3 v3MaxO = pRigidBody->GetMaxGlobal();
	vector3 v3MinO = pRigidBody->GetMinGlobal();

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

//Display based on index and color
void MyOctant::Display(uint a_nIndex, vector3 a_v3Color)
{
	if (m_uID == a_nIndex) {
		m_pMeshMngr->AddWireCubeToRenderList(glm::translate(IDENTITY_M4, m_v3Center) * glm::scale(vector3(m_fSize)), a_v3Color, RENDER_WIRE);
		return;
	}

	for (int i = 0; i < m_uChildren; i++) {
		m_pChild[i]->Display(a_nIndex);
	}
}

//Renders cubes
void MyOctant::Display(vector3 a_v3Color)
{
	for (int i = 0; i < m_uChildren; i++) {
		m_pChild[i]->Display(a_v3Color);
	}

	m_pMeshMngr->AddWireCubeToRenderList(glm::translate(IDENTITY_M4, m_v3Center) * glm::scale(vector3(m_fSize)), a_v3Color, RENDER_WIRE);
}

//Renders octree leaves
void MyOctant::DisplayLeafs(vector3 a_v3Color)
{
	uint leaves = m_lChild.size();
	for (int i = 0; i < leaves; i++) {
		m_lChild[i]->DisplayLeafs(a_v3Color);
	}

	m_pMeshMngr->AddWireCubeToRenderList(glm::translate(IDENTITY_M4, m_v3Center) * glm::scale(vector3(m_fSize)), a_v3Color, RENDER_WIRE);
}

//clears not only our entity list but the entity list of our children
void MyOctant::ClearEntityList(void)
{
	for (int i = 0; i < m_uChildren; i++) {
		m_pChild[i]->ClearEntityList();
	}

	m_EntityList.clear();
}

//Divides octree into more sections
void MyOctant::Subdivide(void)
{
	//if there is a lower level
	if (m_uChildren != 0)
		return;
	
	if (m_uLevel >= m_uMaxLevel)
		return;

	//create new sub division
	vector3 v3Center = m_v3Center;
	m_uChildren = 8;
	//float fSize = m_fSize / 4.0f;
	float size = (m_fSize / 4.0f) * 2.0f;
	

	//Child Number 1
	v3Center.x -= size/(2.0f);
	v3Center.y -= size / (2.0f);
	v3Center.z -= size / (2.0f);
	m_pChild[0] = new MyOctant(v3Center, size);

	//Child Number 2
	v3Center.x += size;
	m_pChild[1] = new MyOctant(v3Center, size);

	//Child Number 3
	v3Center.z += size;
	m_pChild[2] = new MyOctant(v3Center, size);

	//Child Number 4
	v3Center.x -= size;
	m_pChild[3] = new MyOctant(v3Center, size);

	//Child Number 5
	v3Center.y += size;
	m_pChild[4] = new MyOctant(v3Center, size);

	//Child Number 6
	v3Center.z -= size;
	m_pChild[5] = new MyOctant(v3Center, size);

	//Child Number 7
	v3Center.x += size;
	m_pChild[6] = new MyOctant(v3Center, size);

	//Child Number 8
	v3Center.z += size;
	m_pChild[7] = new MyOctant(v3Center, size);

	//Setting values for all the children
	for (int i = 0; i < 8; i++) {
		m_pChild[i]->m_pParent = this;
		m_pChild[i]->m_pRoot = m_pRoot;
		m_pChild[i]->m_uLevel = m_uLevel + 1;
		if (m_pChild[i]->ContainsMoreThan(m_uIdealEntityCount))
			m_pChild[i]->Subdivide();
	}

}

//Gets child at a given index if it exists
MyOctant * MyOctant::GetChild(uint a_nChild)
{
	//if child doesn't exist, return nullptr
	if (a_nChild > 7)
		return nullptr;
	return m_pChild[a_nChild];
}

//gets octant parent
MyOctant * MyOctant::GetParent(void)
{
	return m_pParent;
}

//determines if current octant is a leaf
bool MyOctant::IsLeaf(void)
{
	return m_uChildren == 8;
}

//determines if there are more than a certain number of entities
bool MyOctant::ContainsMoreThan(uint a_nEntities)
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

//its a killall
void MyOctant::KillBranches(void)
{
	for (int i = 0; i < m_uChildren; i++) {
		m_pChild[i]->KillBranches(); //recurses through all possible nodes and destroys all of them
		delete m_pChild[i];
		m_pChild[i] = nullptr;
	}

	m_uChildren = 0;

}

//resets and calls octree creation
void MyOctant::ConstructTree(u_int a_nMaxLevel)
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

//Individualized ID assignment
void MyOctant::AssignIDtoEntity(void)
{
	//recursive assignment of IDs to children
	for (int i = 0; i < m_uChildren; i++) {
		m_pChild[i]->AssignIDtoEntity();
	}

	//if there are no children
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
MyOctant::MyOctant(uint a_nMaxLevel, uint a_nIdealEntityCount)
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

	//Getting mins and maxes sorted
	std::vector<vector3> lMinMax;

	//creating vector of mins and maxes for all the rigidbodies
	uint nObjects = m_pEntityMngr->GetEntityCount();
	for (int i = 0; i < nObjects; i++) {
		MyEntity* pEntity = m_pEntityMngr->GetEntity(i);
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

	//setting newly found center, min, and max
	m_fSize = fMax * 2.0f;
	m_v3Center = v3Center;
	m_v3Min = m_v3Center - (vector3(fMax));
	m_v3Max = m_v3Center + (vector3(fMax));

	//incrementing octant count
	m_uOctantCount++;

	//Creates Octree after variable initialization
	ConstructTree(m_uMaxLevel);
}

//Parameterized
MyOctant::MyOctant(vector3 a_v3Center, float a_fSize) {
	Init();
	m_v3Center = a_v3Center;
	m_fSize = a_fSize;

	m_v3Min = m_v3Center - (vector3(m_fSize) / 2.0f);
	m_v3Max = m_v3Center + (vector3(m_fSize) / 2.0f);

	m_uOctantCount++;
}

//Copy Constructor
MyOctant::MyOctant(MyOctant const& other) {
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

//Copy Assignment
MyOctant& MyOctant::operator=(MyOctant const& other) {
	if (this != &other) {
		Release();
		Init();
		MyOctant temp(other);
		Swap(temp);
	}

	return *this;
}

//Destructor
MyOctant::~MyOctant()
{
	Release();
}


